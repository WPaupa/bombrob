#include "event.h"
#include "socket_utils.h"

using namespace boost::asio::ip;

SockStream &operator>>(SockStream &sock, BombPlacedEvent &event) {
    return sock >> event.id >> event.position;
}

SockStream &operator<<(SockStream &sock, const BombPlacedEvent &event) {
    return sock << event.id << event.position;
}

SockStream &operator>>(SockStream &sock, BombExplodedEvent &event) {
    return sock >> event.id >> event.robots_destroyed >> event.blocks_destroyed;
}

SockStream &operator<<(SockStream &sock, const BombExplodedEvent &event) {
    return sock << event.id << event.robots_destroyed << event.blocks_destroyed;
}

SockStream &operator>>(SockStream &sock, PlayerMovedEvent &event) {
    return sock >> event.id >> event.position;
}

SockStream &operator<<(SockStream &sock, const PlayerMovedEvent &event) {
    return sock << event.id << event.position;
}

SockStream &operator>>(SockStream &sock, BlockPlacedEvent &event) {
    return sock >> event.position;
}

SockStream &operator<<(SockStream &sock, const BlockPlacedEvent &event) {
    return sock << event.position;
}

// Najpierw wczytujemy typ wydarzenia. Jeśli jest błędne,
// to się wywalamy. Jeśli nie, to konstruujemy odpowiedni
// obiekt, a potem wywołujemy odpowiedni wariant
// metody go wczytującej.
SockStream &operator>>(SockStream &sock, Event &event) {
    EventEnum type;
    sock >> type;
    switch (type) {
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
        default:
            throw WrongMessage("Wrong event type!");
    }
    std::visit([&sock](auto &v) {
        sock >> v;
    }, event);
    return sock;
}

SockStream &operator<<(SockStream &sock, const Event &event) {
    auto type(static_cast<EventEnum>(event.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, event);
    return sock;
}