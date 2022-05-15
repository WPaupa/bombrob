#ifndef BOMBOWE_ROBOTY_SOCKETUTILS_H
#define BOMBOWE_ROBOTY_SOCKETUTILS_H
#include <boost/asio.hpp>
#include <map>
#include <vector>
#include "types.h"
#include <variant>

boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &, uint8_t &);
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &, const uint8_t &);

boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &, uint16_t &);
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &, const uint16_t &);

boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &, uint32_t &);
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &, const uint32_t &);

boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &, std::string &);
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &, const std::string &);

template<typename T>
boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &sock, std::vector<T> &bytes) {
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
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &sock, const std::vector<T> &bytes) {
    if (bytes.size() > UINT32_MAX)
        throw std::length_error("vector");
    auto size = static_cast<uint32_t>(bytes.size());
    sock << size;
    for (auto &el : bytes)
        sock << el;
    return sock;
}

template<typename T1, typename T2>
boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &sock, std::map<T1, T2> &bytes) {
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
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &sock, const std::map<T1, T2> &bytes) {
    if (bytes.size() > UINT32_MAX)
        throw std::length_error("map");
    auto size = static_cast<uint32_t>(bytes.size());
    sock << size;
    for (auto &[key, val] : bytes)
        sock << key << val;
    return sock;
}

boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &, Bomb &);
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &, const Bomb &);

boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &, Position &);
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &, const Position &);

boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket &, Player &);
boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket &, const Player &);

template<typename T>
typename std::enable_if<std::is_enum<T>::value, boost::asio::ip::tcp::socket>::type &
operator>>(boost::asio::ip::tcp::socket &sock, T &bytes) {
    uint8_t bytes_uint;
    sock >> bytes_uint;
    bytes = T(bytes_uint);
    return sock;
}
template<typename T>
typename std::enable_if<std::is_enum<T>::value, boost::asio::ip::tcp::socket>::type &
operator<<(boost::asio::ip::tcp::socket &sock, const T &bytes) {
    auto bytes_uint = static_cast<uint8_t>(bytes);
    sock << bytes_uint;
    return sock;
}

#endif //BOMBOWE_ROBOTY_SOCKETUTILS_H
