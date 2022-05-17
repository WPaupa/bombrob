#ifndef BOMBOWE_ROBOTY_TYPES_H
#define BOMBOWE_ROBOTY_TYPES_H
#include <boost/asio.hpp>

enum class Direction : uint8_t {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
};

using BombId = uint32_t;

class Position {
public:
    uint16_t x;
    uint16_t y;
    auto operator<=>(const Position &other) const = default;
    Position(uint16_t x, uint16_t y) : x(x), y(y) {}
    Position() = default;
    Position(const Position &other, Direction direction, uint16_t offset) {
        x = other.x;
        y = other.y;
        switch (direction) {
            case Direction::Up:
                y += offset;
                break;
            case Direction::Right:
                x += offset;
                break;
            case Direction::Down:
                y -= offset;
                break;
            case Direction::Left:
                x -= offset;
                break;
        }
    }
};

class Bomb {
public:
    Position position;
    uint16_t timer;
};

using PlayerId = uint8_t;

class Player {
public:
    std::string name;
    std::string address;
};

using Score = uint32_t;

#endif //BOMBOWE_ROBOTY_TYPES_H
