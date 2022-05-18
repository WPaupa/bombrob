#ifndef BOMBOWE_ROBOTY_SOCKSTREAM_H
#define BOMBOWE_ROBOTY_SOCKSTREAM_H
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <utility>
#define UDP_DGRAM_SIZE 65507

class SockStream;

template<typename T>
void readfrom(SockStream &sock, T &bytes, size_t size = sizeof(T));

template<typename T>
void writeto(SockStream &sock, const T &bytes, size_t size = sizeof(T));

class SockStream {
private:
    virtual void recv(char *bytes, size_t size) = 0;
    virtual void send(const char *bytes, size_t size) = 0;
    template <typename T>
    friend void readfrom(SockStream &, T &, size_t);
    template<typename T>
    friend void writeto(SockStream &, const T &, size_t);
public:
    virtual void flush() = 0;
};


class TCPSockStream : public SockStream {
private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver::results_type endpoints;
public:
    explicit TCPSockStream(const std::string &address)
        : io_context(), resolver(io_context), socket(io_context) {
        size_t port_start = address.find_last_of(':');
        endpoints = resolver.resolve(address.substr(0, port_start), address.substr(port_start + 1));
        boost::asio::connect(socket, endpoints);

        boost::asio::ip::tcp::no_delay option(true);
        socket.set_option(option);
    }
    void recv(char *bytes, size_t size) override {
        auto buff = boost::asio::buffer(bytes, size);
        boost::system::error_code ec;
        boost::asio::read(socket, buff, boost::asio::transfer_exactly(size), ec);
        if (ec)
            throw boost::system::system_error(ec);
    }
    void send(const char *bytes, size_t size) override {
        auto buff = boost::asio::buffer(bytes, size);
        boost::system::error_code ec;
        boost::asio::write(socket, buff, boost::asio::transfer_exactly(size), ec);
        if (ec)
            throw boost::system::system_error(ec);
    }
    void flush() override {}
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
    UDPSockStream(const std::string &address, uint16_t port)
        : io_context(), resolver(io_context), socket(io_context), read_buf(), write_buf(),
        read_pos(read_buf), write_pos(write_buf), read_size(0), write_size(0),
        read_started(false), write_started(false) {
        size_t port_start = address.find_last_of(':');
        endpoint = resolver.resolve(address.substr(0, port_start), address.substr(port_start + 1))->endpoint();
        socket.open(endpoint.protocol());
        boost::asio::ip::udp::endpoint local_end_point(boost::asio::ip::address::from_string("localhost"), port);
        socket.bind(local_end_point);
    }

    void recv(char *bytes, size_t size) override {
        if (!read_started) {
            auto buff = boost::asio::buffer(read_buf, UDP_DGRAM_SIZE);
            boost::asio::ip::udp::endpoint recv_endpoint;
            read_size = socket.receive_from(buff, recv_endpoint);
            // if recv_endpoint != endpoint then jajco
            read_started = true;
        }
        if (read_pos + size > read_buf + read_size)
            throw std::length_error("Datagram (too long)");
        memcpy(bytes, read_pos, size);
        read_pos += size;
    }

    void send(const char *bytes, size_t size) override {
        if (!write_started) {
            write_size = 0;
            write_started = true;
        }
        if (write_pos + size > write_buf + UDP_DGRAM_SIZE)
            throw std::length_error("Datagram exceeds size");
        memcpy(write_pos, bytes, size);
        write_pos += size;
    }

    void flush() override {
        if (read_started && write_started)
            throw std::runtime_error("Reading while writing");
        else if (read_started) {
            if (read_pos != read_buf + read_size)
                throw std::length_error("Datagram");
            read_pos = read_buf;
            read_started = false;
        } else if (write_started) {
            auto buff = boost::asio::buffer(write_buf, write_size);
            socket.send_to(buff, endpoint);
            write_pos = write_buf;
            write_started = false;
        } else throw std::runtime_error("Flushing without io");
    }
};

template<typename T>
void readfrom(SockStream &sock, T &bytes, size_t size) {
    sock.recv(reinterpret_cast<char *>(&bytes), size);
}

template<typename T>
void writeto(SockStream &sock, const T &bytes, size_t size) {
    sock.send(reinterpret_cast<const char *>(&bytes), size);
}

#endif //BOMBOWE_ROBOTY_SOCKSTREAM_H
