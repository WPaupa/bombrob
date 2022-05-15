#ifndef BOMBOWE_ROBOTY_EVENT_H
#define BOMBOWE_ROBOTY_EVENT_H
#include <variant>
#include "types.h"
#include <vector>
#include <boost/asio.hpp>

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
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, BombPlacedEvent &);
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const BombPlacedEvent &);
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
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, BombExplodedEvent &);
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const BombExplodedEvent &);
};

class PlayerMovedEvent {
private:
    PlayerId id;
    Position position;
public:
    PlayerMovedEvent() = default;
    PlayerMovedEvent(PlayerId id, Position position) : id(id), position(position) {}
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, PlayerMovedEvent &);
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const PlayerMovedEvent &);
};

class BlockPlacedEvent {
private:
    Position position;
public:
    BlockPlacedEvent() = default;
    explicit BlockPlacedEvent(Position position) : position(position) {}
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, BlockPlacedEvent &);
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const BlockPlacedEvent &);
};

using Event = std::variant<BombPlacedEvent, BombExplodedEvent, PlayerMovedEvent, BlockPlacedEvent>;

boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, Event &);
boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const Event &);

#endif //BOMBOWE_ROBOTY_EVENT_H
