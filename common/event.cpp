#include "event.h"

using namespace boost::asio::ip;

tcp::socket &operator>>(tcp::socket &sock, BombPlacedEvent &event) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const BombPlacedEvent &event) {
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, BombExplodedEvent &event) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const BombExplodedEvent &event) {
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, PlayerMovedEvent &event) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const PlayerMovedEvent &event) {
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, BlockPlacedEvent &event) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const BlockPlacedEvent &event) {
    return sock;
}