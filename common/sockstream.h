#ifndef BOMBOWE_ROBOTY_SOCKSTREAM_H
#define BOMBOWE_ROBOTY_SOCKSTREAM_H
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include <utility>
#include "exceptions.h"
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
    virtual void flushIn() = 0;
    virtual void flushOut() = 0;
    virtual void stop() = 0;
};


class TCPSockStream : public SockStream {
private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver::results_type endpoints;
public:
    explicit TCPSockStream(const std::string &address)
        : io_context(), resolver(io_context), acceptor(io_context), socket(io_context) {
        size_t port_start = address.find_last_of(':');
        endpoints = resolver.resolve(address.substr(0, port_start), address.substr(port_start + 1));
        boost::asio::connect(socket, endpoints);

        boost::asio::ip::tcp::no_delay option(true);
        socket.set_option(option);
    }
    explicit TCPSockStream(uint16_t port)
            : io_context(), resolver(io_context),
              acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port)),
              socket(io_context) {
        acceptor.accept(socket);
        boost::asio::ip::tcp::no_delay option(true);
        socket.set_option(option);
    }
    void recv(char *bytes, size_t size) override {
        boost::system::error_code ec;
        size_t read_size = 0;
        while (read_size < size) {
            read_size += boost::asio::read(socket, boost::asio::buffer(bytes + read_size, size - read_size),
                                           boost::asio::transfer_exactly(size - read_size), ec);
            if (ec)
                throw boost::system::system_error(ec);
        }
        fprintf(stderr, "Received %zu bytes via TCP: %.*s ( ", read_size, (int)read_size, bytes);
        for (size_t i = 0; i < size; i++)
            fprintf(stderr, "%hhu ", bytes[i]);
        fprintf(stderr, ")\n");
    }
    void send(const char *bytes, size_t size) override {
        auto buff = boost::asio::buffer(bytes, size);
        boost::system::error_code ec;
        boost::asio::write(socket, buff, boost::asio::transfer_exactly(size), ec);
        fprintf(stderr, "Sent %zu bytes via TCP: %.*s ( ", size, (int)size, bytes);
        for (size_t i = 0; i < size; i++)
            fprintf(stderr, "%hhu ", bytes[i]);
        fprintf(stderr, ")\n");
        if (ec)
            throw boost::system::system_error(ec);
    }
    void flushIn() override {}
    void flushOut() override {}
    void stop() override {
        boost::system::error_code ec;
	socket.close(ec);
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
    UDPSockStream(const std::string &address, uint16_t port)
        : io_context(), resolver(io_context), socket(io_context), read_buf(), write_buf(),
        read_pos(read_buf), write_pos(write_buf), read_size(0), write_size(0),
        read_started(false), write_started(false) {
        size_t port_start = address.find_last_of(':');
        endpoint = resolver.resolve(address.substr(0, port_start), address.substr(port_start + 1))->endpoint();
        socket.open(boost::asio::ip::udp::v6());
        boost::asio::ip::udp::endpoint local_end_point(boost::asio::ip::udp::v6(), port);
        socket.bind(local_end_point);
    }

    void recv(char *bytes, size_t size) override {
        if (!read_started) {
            read_pos = read_buf;
            auto buff = boost::asio::buffer(read_buf, UDP_DGRAM_SIZE);
            boost::system::error_code ec;
            read_size = socket.receive(buff, 0, ec);
            fprintf(stderr, "Received %zu bytes via UDP: %.*s ( ", read_size, (int)read_size, read_buf);
            for (size_t i = 0; i < read_size; i++)
                fprintf(stderr, "%hhu ", read_buf[i]);
            fprintf(stderr, ")\n");
            if (ec)
                throw boost::system::system_error(ec);
            // if recv_endpoint != endpoint then jajco
            read_started = true;
        }
        if (read_pos + size > read_buf + read_size) {
            read_started = false;
            throw WrongMessage("Datagram (too long)");
        }
        memcpy(bytes, read_pos, size);
        read_pos += size;
    }

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

    void flushIn() override {
        if (read_started) {
            read_started = false;
            if (read_pos != read_buf + read_size)
                throw WrongMessage("Datagram");
        } else throw std::runtime_error("Flushing in without io");
    }

    void flushOut() override {
        if (write_started) {
            auto buff = boost::asio::buffer(write_buf, write_size);
            socket.send_to(buff, endpoint);
            fprintf(stderr, "Sent %zu bytes via UDP: %.*s ( ", write_size, (int)write_size, write_buf);
            for (size_t i = 0; i < write_size; i++)
                fprintf(stderr, "%hhu ", write_buf[i]);
            fprintf(stderr, ")\n");
            write_started = false;
        } else throw std::runtime_error("Flushing out without io");
    }

    void stop() override {
        boost::system::error_code ec;
        socket.shutdown(boost::asio::ip::udp::socket::shutdown_both, ec);
        socket.close(ec);
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
