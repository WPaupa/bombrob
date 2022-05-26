#ifndef BOMBOWE_ROBOTY_SOCKET_UTILS_H
#define BOMBOWE_ROBOTY_SOCKET_UTILS_H

#include <boost/asio.hpp>
#include <map>
#include <vector>
#include "types.h"
#include <variant>
#include "sock_stream.h"

SockStream &operator>>(SockStream &, uint8_t &);
SockStream &operator<<(SockStream &, const uint8_t &);

SockStream &operator>>(SockStream &, uint16_t &);
SockStream &operator<<(SockStream &, const uint16_t &);

SockStream &operator>>(SockStream &, uint32_t &);
SockStream &operator<<(SockStream &, const uint32_t &);

SockStream &operator>>(SockStream &, std::string &);
SockStream &operator<<(SockStream &, const std::string &);

template<typename T>
SockStream &operator>>(SockStream &sock, std::vector<T> &bytes) {
    uint32_t size;
    sock >> size;
    for (uint32_t i = 0; i < size; i++) {
        T el;
        sock >> el;
        bytes.push_back(el);
    }
    return sock;
}

template<typename T>
SockStream &operator<<(SockStream &sock, const std::vector<T> &bytes) {
    if (bytes.size() > UINT32_MAX)
        throw std::length_error("vector");
    auto size = static_cast<uint32_t>(bytes.size());
    sock << size;
    for (auto &el : bytes)
        sock << el;
    return sock;
}

template<typename T1, typename T2>
SockStream &operator>>(SockStream &sock, std::map<T1, T2> &bytes) {
    uint32_t size;
    sock >> size;
    for (uint32_t i = 0; i < size; i++) {
        T1 key;
        sock >> key;
        T2 val;
        sock >> val;
        bytes.insert({key, val});
    }
    return sock;
}

template<typename T1, typename T2>
SockStream &operator<<(SockStream &sock, const std::map<T1, T2> &bytes) {
    if (bytes.size() > UINT32_MAX)
        throw std::length_error("map");
    auto size = static_cast<uint32_t>(bytes.size());
    sock << size;
    for (auto &[key, val] : bytes)
        sock << key << val;
    return sock;
}

SockStream &operator>>(SockStream &, Bomb &);
SockStream &operator<<(SockStream &, const Bomb &);

SockStream &operator>>(SockStream &, Position &);
SockStream &operator<<(SockStream &, const Position &);

SockStream &operator>>(SockStream &, Player &);
SockStream &operator<<(SockStream &, const Player &);

template<typename T>
typename std::enable_if<std::is_enum<T>::value, SockStream>::type &
operator>>(SockStream &sock, T &bytes) {
    uint8_t bytes_uint;
    sock >> bytes_uint;
    bytes = T(bytes_uint);
    return sock;
}

template<typename T>
typename std::enable_if<std::is_enum<T>::value, SockStream>::type &
operator<<(SockStream &sock, const T &bytes) {
    auto bytes_uint = static_cast<uint8_t>(bytes);
    sock << bytes_uint;
    return sock;
}

#endif //BOMBOWE_ROBOTY_SOCKET_UTILS_H
