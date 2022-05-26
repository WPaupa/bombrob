#ifndef BOMBOWE_ROBOTY_EVENT_H
#define BOMBOWE_ROBOTY_EVENT_H

#include <utility>
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

    [[nodiscard]] BombId getId() const {
        return id;
    }

    [[nodiscard]] Position getPosition() const {
        return position;
    }
};

class BombExplodedEvent {
private:
    BombId id;
    std::vector<PlayerId> robots_destroyed;
    std::vector<Position> blocks_destroyed;
public:
    BombExplodedEvent() = default;

    BombExplodedEvent(BombId id, std::vector<PlayerId> robots_destroyed,
                      std::vector<Position> blocks_destroyed)
            : id(id), robots_destroyed(std::move(robots_destroyed)),
              blocks_destroyed(std::move(blocks_destroyed)) {}

    friend SockStream &operator>>(SockStream &, BombExplodedEvent &);
    friend SockStream &operator<<(SockStream &, const BombExplodedEvent &);

    [[nodiscard]] BombId getId() const {
        return id;
    }

    [[nodiscard]] std::vector<PlayerId> getRobotsDestroyed() const {
        return robots_destroyed;
    }

    [[nodiscard]] std::vector<Position> getBlocksDestroyed() const {
        return blocks_destroyed;
    }
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

    [[nodiscard]] PlayerId getId() const {
        return id;
    }

    [[nodiscard]] Position getPosition() const {
        return position;
    }
};

class BlockPlacedEvent {
private:
    Position position;
public:
    BlockPlacedEvent() = default;

    explicit BlockPlacedEvent(Position position) : position(position) {}

    friend SockStream &operator>>(SockStream &, BlockPlacedEvent &);
    friend SockStream &operator<<(SockStream &, const BlockPlacedEvent &);

    [[nodiscard]] Position getPosition() const {
        return position;
    }
};

using Event = std::variant<BombPlacedEvent, BombExplodedEvent, PlayerMovedEvent, BlockPlacedEvent>;

inline EventEnum eventType(const Event &event) {
    return std::visit([](auto &&v) {
        typedef std::decay_t<typeof(v)> T;
        if constexpr (std::is_same_v<T, BombPlacedEvent>)
            return EventEnum::BombPlaced;
        else if constexpr (std::is_same_v<T, BombExplodedEvent>)
            return EventEnum::BombExploded;
        else if constexpr (std::is_same_v<T, PlayerMovedEvent>)
            return EventEnum::PlayerMoved;
        else if constexpr (std::is_same_v<T, BlockPlacedEvent>)
            return EventEnum::BlockPlaced;
    }, event);
}

SockStream &operator>>(SockStream &, Event &);
SockStream &operator<<(SockStream &, const Event &);

#endif //BOMBOWE_ROBOTY_EVENT_H
