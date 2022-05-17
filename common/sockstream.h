#ifndef BOMBOWE_ROBOTY_SOCKSTREAM_H
#define BOMBOWE_ROBOTY_SOCKSTREAM_H
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <memory>

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
};

class TCPSockStream : SockStream {
private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver::results_type endpoints;
public:
    explicit TCPSockStream(boost::asio::ip::tcp::resolver::results_type &endpoints)
        : io_context(), resolver(io_context), socket(io_context), endpoints(endpoints) {
        boost::asio::ip::tcp::no_delay option(true);
        socket.set_option(option);
        boost::asio::connect(socket, endpoints);
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
};

class UDPSockStream : SockStream {
private:
    boost::asio::io_context io_context;
    boost::asio::ip::udp::resolver resolver;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint endpoint;
public:
    explicit UDPSockStream(boost::asio::ip::udp::endpoint &endpoint)
        : io_context(), resolver(io_context), socket(io_context), endpoint(endpoint) {
        socket.open(boost::asio::ip::udp::v4());
    }

    void recv(char *bytes, size_t size) override {
        auto buff = boost::asio::buffer(bytes, size);
        size_t read_size = socket.receive_from(buff, endpoint);
        if (read_size != size)
            throw std::length_error("recv");
    }

    void send(const char *bytes, size_t size) override {
        auto buff = boost::asio::buffer(bytes, size);
        socket.send_to(buff, endpoint);
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
