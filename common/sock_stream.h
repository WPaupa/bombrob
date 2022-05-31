#ifndef BOMBOWE_ROBOTY_SOCK_STREAM_H
#define BOMBOWE_ROBOTY_SOCK_STREAM_H

#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <utility>
#include "exceptions.h"
#include <sstream>

#define UDP_DGRAM_SIZE 65507

// Klasa strumienia z gniazda. Opakowuje gniazdo tak, żeby można
// było z niego dokonywać operacji wejścia/wyjścia i zamykać je,
// niezależnie od jego protokołu.
class SockStream;

// Szablony funkcji do czytania ze strumienia i pisania do strumienia.
// Przyjmują argument typu T i interpretują go jako element pod wskaźnikiem na
// (domyślnie sizeof(T)) bajtów.

template<typename T>
void readFrom(SockStream &sock, T &bytes, size_t size = sizeof(T));

template<typename T>
void writeTo(SockStream &sock, const T &bytes, size_t size = sizeof(T));

class SockStream {
private:
    virtual void receive(char *bytes, size_t size) = 0;
    virtual void send(const char *bytes, size_t size) = 0;

    template<typename T>
    friend void readFrom(SockStream &sock, T &bytes, size_t size);

    template<typename T>
    friend void writeTo(SockStream &sock, const T &bytes, size_t size);

public:
    // Metody zatwierdzające koniec wiadomości, przydatne tylko do UDP.
    virtual void flushIn() = 0;
    virtual void flushOut() = 0;

    // Metoda kończąca nasłuchiwanie na gnieździe i zamykająca to gniazdo.
    virtual void stop() = 0;
};


class TCPSockStream : public SockStream {
protected:
    std::shared_ptr<boost::asio::io_context> io_context;
    boost::asio::ip::tcp::socket socket;
    TCPSockStream() : io_context(std::make_shared<boost::asio::io_context>()), socket(*io_context) {}
    explicit TCPSockStream(std::shared_ptr<boost::asio::io_context> context_ptr)
        : io_context(std::move(context_ptr)), socket(*io_context) {}
public:
    // Metoda pobierająca dane z gniazda. W pętli czyta
    // kolejne bajty, aż nie natrafi na błąd lub nie
    // przeczyta całej wiadomości.
    void receive(char *bytes, size_t size) override {
        boost::system::error_code ec;
        size_t read_size = 0;
        while (read_size < size) {
            read_size += boost::asio::read(socket,
                                           boost::asio::buffer(bytes + read_size, size - read_size),
                                           boost::asio::transfer_exactly(size - read_size), ec);
            if (ec)
                throw boost::system::system_error(ec);
        }
        DEBUG("Received %zu bytes via TCP: %.*s ( ", read_size, (int) read_size, bytes);
        for (size_t i = 0; i < size; i++)
            DEBUG("%hhu ", bytes[i]);
        DEBUG(")\n");
    }

    void send(const char *bytes, size_t size) override {
        auto buff = boost::asio::buffer(bytes, size);
        boost::system::error_code ec;
        boost::asio::write(socket, buff, boost::asio::transfer_exactly(size), ec);
        DEBUG("Sent %zu bytes via TCP: %.*s ( ", size, (int) size, bytes);
        for (size_t i = 0; i < size; i++)
            DEBUG("%hhu ", bytes[i]);
        DEBUG(")\n");
        if (ec)
            throw boost::system::system_error(ec);
    }

    void flushIn() override {}

    void flushOut() override {}

    // Najpierw wyłączamy nasłuchiwanie, a potem zamykamy gniazdo.
    void stop() override {
        boost::system::error_code ec;
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        socket.close(ec);
    }
};

class TCPClientSockStream final : public TCPSockStream {
private:
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::resolver::results_type endpoints;
public:
    explicit TCPClientSockStream(const std::string &address)
            : TCPSockStream(), resolver(*io_context) {
        size_t port_start = address.find_last_of(':');
        endpoints = resolver.resolve(address.substr(0, port_start), address.substr(port_start + 1));
        boost::asio::connect(socket, endpoints);

        boost::asio::ip::tcp::no_delay option(true);
        socket.set_option(option);
    }
};

struct TCPServerSockStreamProvider {
    std::shared_ptr<boost::asio::io_context> io_context;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    explicit TCPServerSockStreamProvider(uint16_t port)
    : io_context(std::make_shared<boost::asio::io_context>()),
      acceptor(std::make_shared<boost::asio::ip::tcp::acceptor>(*io_context,
               boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v6(), port})) {}
};

class TCPServerSockStream final : public TCPSockStream {
private:
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
public:
    explicit TCPServerSockStream(const TCPServerSockStreamProvider &p)
    : TCPSockStream(p.io_context), acceptor(p.acceptor) {
        acceptor->accept(socket);

        boost::asio::ip::tcp::no_delay option(true);
        socket.set_option(option);
    }

    std::string getAddress() {
        std::ostringstream os;
        os << socket.remote_endpoint();
        return os.str();
    }
};


class UDPSockStream : public SockStream {
private:
    boost::asio::io_context io_context;
    boost::asio::ip::udp::resolver resolver;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint endpoint;
    char read_buf[UDP_DGRAM_SIZE];
    char write_buf[UDP_DGRAM_SIZE];
    char *read_pos, *write_pos;
    size_t read_size, write_size;
    bool read_started, write_started;
public:

    // Żeby stworzyć strumień po UDP, musimy podać docelowy
    // adres i nasz port. Wtedy z docelowego adresu tworzymy punkt końcowy,
    // otwieramy gniazdo i zaczynamy słuchać na naszym porcie.
    // Korzystamy z protokołu v6, bo obsługuje on też v4,
    // a na odwrót nie.
    UDPSockStream(const std::string &address, uint16_t port)
            : io_context(), resolver(io_context), socket(io_context),
              read_buf(), write_buf(), read_pos(read_buf),
              write_pos(write_buf), read_size(0), write_size(0),
              read_started(false), write_started(false) {
        size_t port_start = address.find_last_of(':');
        endpoint = resolver.resolve(address.substr(0, port_start),
                                    address.substr(port_start + 1))->endpoint();
        socket.open(boost::asio::ip::udp::v6());
        boost::asio::ip::udp::endpoint local_end_point(boost::asio::ip::udp::v6(), port);
        socket.bind(local_end_point);
    }

    // Jeśli nie zaczęliśmy jeszcze czytać, to pobieramy nowy datagram
    // do buforu i zerujemy odpowiednie zmienne. Jeśli przeczytaliśmy
    // za dużo, wyrzucamy błąd. W przeciwnym wypadku bierzemy
    // dane z bufora.
    void receive(char *bytes, size_t size) override {
        if (!read_started) {
            read_pos = read_buf;
            auto buff = boost::asio::buffer(read_buf, UDP_DGRAM_SIZE);
            boost::system::error_code ec;
            read_size = socket.receive(buff, 0, ec);
            DEBUG("Received %zu bytes via UDP: %.*s ( ", read_size, (int) read_size,
                    read_buf);
            for (size_t i = 0; i < read_size; i++)
                DEBUG("%hhu ", read_buf[i]);
            DEBUG(")\n");
            if (ec)
                throw boost::system::system_error(ec);
            read_started = true;
        }
        if (read_pos + size > read_buf + read_size) {
            read_started = false;
            throw WrongMessage("Datagram (too long)");
        }
        memcpy(bytes, read_pos, size);
        read_pos += size;
    }

    // Jeśli nie zaczęliśmy jeszcze pisać, to zaczynamy
    // (poprzez wyzerowanie odpowiednich zmiennych). Jeśli
    // napisaliśmy za dużo, to się wywalamy. W przeciwnym
    // wypadku dopisujemy dane do bufora.
    void send(const char *bytes, size_t size) override {
        if (!write_started) {
            write_size = 0;
            write_pos = write_buf;
            write_started = true;
        }
        if (write_pos + size > write_buf + UDP_DGRAM_SIZE) {
            write_started = false;
            throw WrongMessage("Datagram exceeds size");
        }
        memcpy(write_pos, bytes, size);
        write_pos += size;
        write_size += size;
    }

    // Jeśli nie odczytaliśmy całego bufora, to się wywalamy.
    void flushIn() override {
        if (read_started) {
            read_started = false;
            if (read_pos != read_buf + read_size)
                throw WrongMessage("Datagram");
        } else throw std::runtime_error("Flushing in without io");
    }

    // Wysyłamy zbudowany datagram.
    void flushOut() override {
        if (write_started) {
            auto buff = boost::asio::buffer(write_buf, write_size);
            socket.send_to(buff, endpoint);
            DEBUG("Sent %zu bytes via UDP: %.*s ( ", write_size, (int) write_size,
                    write_buf);
            for (size_t i = 0; i < write_size; i++)
                DEBUG("%hhu ", write_buf[i]);
            DEBUG(")\n");
            write_started = false;
        } else throw std::runtime_error("Flushing out without io");
    }

    void stop() override {
        boost::system::error_code ec;
        socket.shutdown(boost::asio::ip::udp::socket::shutdown_both, ec);
        socket.close(ec);
    }
};

// Poniższe funkcje zgodnie ze swoim przeznaczeniem interpretują
// argument jako element pod wskaźnikiem na size bajtów.
// Osiąga to przez (o zgrozo) reinterpret_cast.
// Musimy stworzyć ten mało sensowny wrapper, który tylko wywołuje
// inną funkcję, bo nie moglibyśmy stworzyć szablonu wirtualnej funkcji.

template<typename T>
void readFrom(SockStream &sock, T &bytes, size_t size) {
    sock.receive(reinterpret_cast<char *>(&bytes), size);
}

template<typename T>
void writeTo(SockStream &sock, const T &bytes, size_t size) {
    sock.send(reinterpret_cast<const char *>(&bytes), size);
}

#endif //BOMBOWE_ROBOTY_SOCK_STREAM_H
