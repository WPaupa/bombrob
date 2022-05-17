#ifndef BOMBOWE_ROBOTY_EVENT_H
#define BOMBOWE_ROBOTY_EVENT_H
#include <variant>
#include "types.h"
#include <vector>
#include <boost/asio.hpp>
#include "sockstream.h"

enum class EventEnum {
    BombPlaced = 0,
    BombExploded = 1,
    PlayerMoved = 2,
    BlockPlaced = 3,
};

class BombPlacedEvent {
private:
    BombId id;
    Position position;
public:
    BombPlacedEvent() = default;
    BombPlacedEvent(BombId id, Position position) : id(id), position(position) {}
    friend SockStream &operator>>(SockStream &, BombPlacedEvent &);
    friend SockStream &operator<<(SockStream &, const BombPlacedEvent &);
};

class BombExplodedEvent {
private:
    BombId id;
    std::vector<PlayerId> robots_destroyed;
    std::vector<Position> blocks_destroyed;
public:
    BombExplodedEvent() = default;
    BombExplodedEvent(BombId id, std::vector<PlayerId> &robots_destroyed, std::vector<Position> &blocks_destroyed)
        : id(id), robots_destroyed(robots_destroyed), blocks_destroyed(blocks_destroyed) {}
    friend SockStream &operator>>(SockStream &, BombExplodedEvent &);
    friend SockStream &operator<<(SockStream &, const BombExplodedEvent &);
};

class PlayerMovedEvent {
private:
    PlayerId id;
    Position position;
public:
    PlayerMovedEvent() = default;
    PlayerMovedEvent(PlayerId id, Position position) : id(id), position(position) {}
    friend SockStream &operator>>(SockStream &, PlayerMovedEvent &);
    friend SockStream &operator<<(SockStream &, const PlayerMovedEvent &);
};

class BlockPlacedEvent {
private:
    Position position;
public:
    BlockPlacedEvent() = default;
    explicit BlockPlacedEvent(Position position) : position(position) {}
    friend SockStream &operator>>(SockStream &, BlockPlacedEvent &);
    friend SockStream &operator<<(SockStream &, const BlockPlacedEvent &);
};

using Event = std::variant<BombPlacedEvent, BombExplodedEvent, PlayerMovedEvent, BlockPlacedEvent>;

SockStream &operator>>(SockStream &, Event &);
SockStream &operator<<(SockStream &, const Event &);

#endif //BOMBOWE_ROBOTY_EVENT_H
