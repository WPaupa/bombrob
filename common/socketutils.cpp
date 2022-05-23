#include "socketutils.h"
#include <boost/endian/conversion.hpp>

using namespace boost;
using namespace boost::endian;
using namespace boost::asio::ip;
using namespace std;

SockStream& operator>>(SockStream &sock, uint8_t &bytes) {
    readfrom(sock, bytes);
    return sock;
}
SockStream& operator<<(SockStream &sock, const uint8_t &bytes) {
    writeto(sock, bytes);
    return sock;
}

SockStream& operator>>(SockStream &sock, uint16_t &bytes) {
    readfrom(sock, bytes);
    bytes = endian_reverse(bytes);
    return sock;
}
SockStream& operator<<(SockStream &sock, const uint16_t &bytes) {
    uint16_t reversed = endian_reverse(bytes);
    writeto(sock, reversed);
    return sock;
}

SockStream& operator>>(SockStream &sock, uint32_t &bytes) {
    readfrom(sock, bytes);
    bytes = endian_reverse(bytes);
    return sock;
}
SockStream& operator<<(SockStream &sock, const uint32_t &bytes) {
    uint32_t reversed = endian_reverse(bytes);
    writeto(sock, reversed);
    return sock;
}

SockStream& operator>>(SockStream &sock, string &bytes) {
    uint8_t size;
    readfrom(sock, size);
    char *bytes_ptr = new char[size];
    readfrom(sock, *bytes_ptr, size);
    bytes = string(bytes_ptr, size);
    delete[] bytes_ptr;
    return sock;
}
SockStream& operator<<(SockStream &sock, const string &bytes) {
    if (bytes.size() > UINT8_MAX)
        throw std::length_error("bytes");
    auto size = static_cast<uint8_t>(bytes.size());
    writeto(sock, size);
    writeto(sock, *bytes.c_str(), size);
    return sock;
}

SockStream& operator>>(SockStream &sock, Bomb &bytes) {
    return sock >> bytes.position >> bytes.timer;
}
SockStream& operator<<(SockStream &sock, const Bomb &bytes) {
    return sock << bytes.position << bytes.timer;
}

SockStream& operator>>(SockStream &sock, Position &bytes) {
    return sock >> bytes.x >> bytes.y;
}
SockStream& operator<<(SockStream &sock, const Position &bytes) {
    return sock << bytes.x << bytes.y;
}

SockStream& operator>>(SockStream &sock, Player &bytes) {
    return sock >> bytes.name >> bytes.address;
}
SockStream& operator<<(SockStream &sock, const Player &bytes) {
    return sock << bytes.name << bytes.address;
}