#ifndef BOMBOWE_ROBOTY_TYPES_H
#define BOMBOWE_ROBOTY_TYPES_H

#include <boost/asio.hpp>

// Typy zdefiniowane w treści.

enum class Direction : uint8_t {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
};
#define DIRECTION_MAX 3

using BombId = uint32_t;

struct Position {
    uint16_t x;
    uint16_t y;

    auto operator<=>(const Position &other) const = default;

    Position(uint16_t x, uint16_t y) : x(x), y(y) {}

    Position() = default;

    // Konstruktor pozycji powstałej z przesunięcia
    // innej pozycji w konkretną stronę o konkretną długość.
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

struct Bomb {
    Position position;
    uint16_t timer;

    Bomb() = default;

    Bomb(Position position, uint16_t timer) : position(position), timer(timer) {}

    auto operator<=>(const Bomb &other) const = default;
};

using PlayerId = uint8_t;

struct Player {
    std::string name;
    std::string address;
};

using Score = uint32_t;

#endif //BOMBOWE_ROBOTY_TYPES_H
