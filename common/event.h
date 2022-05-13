#ifndef BOMBOWE_ROBOTY_EVENT_H
#define BOMBOWE_ROBOTY_EVENT_H
#include "variant.h"
#include "types.h"
#include <vector>
#include <boost/asio.hpp>

enum class EventEnum {
    BombPlaced = 0,
    BombExploded = 1,
    PlayerMoved = 2,
    BlockPlaced = 3,
};

class EventBase {
public:
    virtual void send(boost::asio::ip::tcp::socket &&socket) = 0;
    virtual void recv(boost::asio::ip::tcp::socket &&socket) = 0;
};

class BombPlacedEvent : EventBase {
private:
    BombId id;
    Position position;
public:
    BombPlacedEvent() = default;
    BombPlacedEvent(BombId id, Position position) : id(id), position(position) {}
    void send(boost::asio::ip::tcp::socket &&socket) override;
    void recv(boost::asio::ip::tcp::socket &&socket) override;
};

class BombExplodedEvent : EventBase {
private:
    BombId id;
    std::vector<PlayerId> robots_destroyed;
    std::vector<Position> blocks_destroyed;
public:
    BombExplodedEvent() = default;
    BombExplodedEvent(BombId id, std::vector<PlayerId> &robots_destroyed, std::vector<Position> &blocks_destroyed)
        : id(id), robots_destroyed(robots_destroyed), blocks_destroyed(blocks_destroyed) {}
    void send(boost::asio::ip::tcp::socket &&socket) override;
    void recv(boost::asio::ip::tcp::socket &&socket) override;
};

class PlayerMovedEvent : EventBase {
private:
    PlayerId id;
    Position position;
public:
    PlayerMovedEvent() = default;
    PlayerMovedEvent(PlayerId id, Position position) : id(id), position(position) {}
    void send(boost::asio::ip::tcp::socket &&socket) override;
    void recv(boost::asio::ip::tcp::socket &&socket) override;
};

class BlockPlacedEvent : EventBase {
private:
    Position position;
public:
    BlockPlacedEvent() = default;
    explicit BlockPlacedEvent(Position position) : position(position) {}
    void send(boost::asio::ip::tcp::socket &&socket) override;
    void recv(boost::asio::ip::tcp::socket &&socket) override;
};

using Event = Variant<EventBase, BombPlacedEvent, BombExplodedEvent, PlayerMovedEvent, BlockPlacedEvent>;

#endif //BOMBOWE_ROBOTY_EVENT_H
