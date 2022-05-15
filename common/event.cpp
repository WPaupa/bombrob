#include "event.h"
#include "socketutils.h"

using namespace boost::asio::ip;

tcp::socket &operator>>(tcp::socket &sock, BombPlacedEvent &event) {
    return sock >> event.id >> event.position;
}

tcp::socket &operator<<(tcp::socket &sock, const BombPlacedEvent &event) {
    return sock << event.id << event.position;
}

tcp::socket &operator>>(tcp::socket &sock, BombExplodedEvent &event) {
    return sock >> event.robots_destroyed >> event.blocks_destroyed;
}

tcp::socket &operator<<(tcp::socket &sock, const BombExplodedEvent &event) {
    return sock << event.robots_destroyed << event.blocks_destroyed;
}

tcp::socket &operator>>(tcp::socket &sock, PlayerMovedEvent &event) {
    return sock >> event.id >> event.position;
}

tcp::socket &operator<<(tcp::socket &sock, const PlayerMovedEvent &event) {
    return sock << event.id << event.position;
}

tcp::socket &operator>>(tcp::socket &sock, BlockPlacedEvent &event) {
    return sock >> event.position;
}

tcp::socket &operator<<(tcp::socket &sock, const BlockPlacedEvent &event) {
    return sock << event.position;
}

tcp::socket &operator>>(tcp::socket &sock, Event &event) {
    EventEnum type;
    sock >> type;
    switch(type) {
        case EventEnum::BombPlaced:
            event.emplace<BombPlacedEvent>();
            break;
        case EventEnum::BombExploded:
            event.emplace<BombExplodedEvent>();
            break;
        case EventEnum::PlayerMoved:
            event.emplace<PlayerMovedEvent>();
            break;
        case EventEnum::BlockPlaced:
            event.emplace<BlockPlacedEvent>();
            break;
    }
    std::visit([&sock](auto &v) {
        sock >> v;
    }, event);
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const Event &event) {
    auto type(static_cast<EventEnum>(event.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, event);
    return sock;
}