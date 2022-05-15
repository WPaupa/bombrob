#include "socketutils.h"
#include <boost/system/system_error.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost;
using namespace boost::endian;
using namespace boost::asio::ip;
using namespace std;

template<typename T>
void readfrom(tcp::socket &sock, T &bytes, size_t size = sizeof(T)) {
    auto buff = asio::buffer(reinterpret_cast<char *>(&bytes), size);
    system::error_code ec;
    read(sock, buff, asio::transfer_exactly(size), ec);
    if (ec)
        throw system_error(ec);
}

template<typename T>
void writeto(tcp::socket &sock, const T &bytes, size_t size = sizeof(T)) {
    auto buff = asio::buffer(reinterpret_cast<const char *>(&bytes), size);
    system::error_code ec;
    write(sock, buff, asio::transfer_exactly(size), ec);
    if (ec)
        throw system_error(ec);
}

tcp::socket& operator>>(tcp::socket &sock, uint8_t &bytes) {
    readfrom(sock, bytes);
    return sock;
}
tcp::socket& operator<<(tcp::socket &sock, const uint8_t &bytes) {
    writeto(sock, bytes);
    return sock;
}

tcp::socket& operator>>(tcp::socket &sock, uint16_t &bytes) {
    readfrom(sock, bytes);
    bytes = endian_reverse(bytes);
    return sock;
}
tcp::socket& operator<<(tcp::socket &sock, const uint16_t &bytes) {
    uint16_t reversed = endian_reverse(bytes);
    writeto(sock, reversed);
    return sock;
}

tcp::socket& operator>>(tcp::socket &sock, uint32_t &bytes) {
    readfrom(sock, bytes);
    bytes = endian_reverse(bytes);
    return sock;
}
tcp::socket& operator<<(tcp::socket &sock, const uint32_t &bytes) {
    uint32_t reversed = endian_reverse(bytes);
    writeto(sock, reversed);
    return sock;
}

tcp::socket& operator>>(tcp::socket &sock, string &bytes) {
    uint8_t size;
    readfrom(sock, size);
    char *bytes_arr;
    readfrom(sock, bytes_arr, size);
    bytes = string(bytes_arr, size);
    return sock;
}
tcp::socket& operator<<(tcp::socket &sock, const string &bytes) {
    if (bytes.size() > UINT8_MAX)
        throw std::length_error(bytes);
    auto size = static_cast<uint8_t>(bytes.size());
    writeto(sock, size);
    const char *bytes_ptr = &bytes[0];
    writeto(sock, bytes_ptr, size);
    return sock;
}

tcp::socket& operator>>(tcp::socket &sock, Bomb &bytes) {
    return sock >> bytes.position >> bytes.timer;
}
tcp::socket& operator<<(tcp::socket &sock, const Bomb &bytes) {
    return sock << bytes.position << bytes.timer;
}

tcp::socket& operator>>(tcp::socket &sock, Position &bytes) {
    return sock >> bytes.x >> bytes.y;
}
tcp::socket& operator<<(tcp::socket &sock, const Position &bytes) {
    return sock << bytes.x << bytes.y;
}

tcp::socket& operator>>(tcp::socket &sock, Player &bytes) {
    return sock >> bytes.name >> bytes.address;
}
tcp::socket& operator<<(tcp::socket &sock, const Player &bytes) {
    return sock << bytes.name << bytes.address;
}