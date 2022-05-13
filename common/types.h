#ifndef BOMBOWE_ROBOTY_TYPES_H
#define BOMBOWE_ROBOTY_TYPES_H

enum class Direction : unsigned char {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
};

using BombId = uint32_t;

class Position {
    uint16_t x;
    uint16_t y;
};

class Bomb {
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
