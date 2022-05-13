#ifndef BOMBOWE_ROBOTY_EVENT_H
#define BOMBOWE_ROBOTY_EVENT_H
#include "variant.h"
#include "types.h"
#include <vector>

enum class EventEnum {
    BombPlaced = 0,
    BombExploded = 1,
    PlayerMoved = 2,
    BlockPlaced = 3,
};

class EventBase {
public:
};

class BombPlacedEvent : EventBase {
private:
    BombId id;
    Position position;
};

class BombExplodedEvent : EventBase {
private:
    BombId id;
    std::vector<PlayerId> robots_destroyed;
    std::vector<Position> blocks_destroyed;
};

class PlayerMovedEvent : EventBase {
private:
    PlayerId id;
    Position position;
};

class BlockPlacedEvent : EventBase {
    Position position;
};

using Event = Variant<EventBase, BombPlacedEvent, BombExplodedEvent, PlayerMovedEvent, BlockPlacedEvent>;

#endif //BOMBOWE_ROBOTY_EVENT_H
