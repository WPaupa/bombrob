#ifndef BOMBOWE_ROBOTY_GAME_STATE_H
#define BOMBOWE_ROBOTY_GAME_STATE_H
#include "../common/types.h"
#include "random.h"
#include <vector>
#include "../common/event.h"
#include <map>
#include "../common/message.h"

class ServerState {
private:
    std::vector<Player> players;
    Random random;
public:
};

class GameState {
private:
    bool started;
    uint16_t turn;
    std::vector<Event> events;
    std::map<PlayerId, Position> player_positions;
    std::map<PlayerId, uint16_t> player_deaths;
    std::map<Position, uint16_t> bombs;
    std::vector<Position> blocks;
public:
    GameState() : started(false), turn(0) {};
    void startGame(uint16_t initial_blocks);
    void addPlayerTurn(ClientMessage &message);
    void updateTurn();
    std::vector<Event> &getEvents();
};


#endif //BOMBOWE_ROBOTY_GAME_STATE_H
