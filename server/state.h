#ifndef BOMBOWE_ROBOTY_GAME_STATE_H
#define BOMBOWE_ROBOTY_GAME_STATE_H

#include "../common/types.h"
#include "random.h"
#include <vector>
#include "../common/event.h"
#include <map>
#include "../common/message.h"
#include "server_options.h"
#include <set>

class GameState;

class ServerState {
private:
    ServerOptions options;
    std::vector<Player> players;
    Random random;
    friend GameState;
public:
    PlayerId addPlayer(Player &player);

    Player getPlayer(PlayerId id);

    PlayerId playerCount();
};

class GameState {
private:
    ServerState &state;
    uint16_t turn;
    std::vector<Event> events;
    std::map<PlayerId, Position> player_positions;
    std::map<PlayerId, uint16_t> player_deaths;
    std::map<BombId, std::pair<Position, uint16_t>> bombs;
    std::map<PlayerId, std::pair<ClientMessageEnum, ClientMessage>> player_moves;
    std::set<Position> blocks;

    void explodeBombs(std::set<Position> &, std::set<PlayerId> &);

    void executePlayerMove(ClientMessage &message, ClientMessageEnum type, PlayerId id);

public:
    explicit GameState(ServerState &state);

    void addPlayerMove(ClientMessage &message, ClientMessageEnum type, Player &player);

    void updateTurn();

    std::vector<Event> &getEvents();
};


#endif //BOMBOWE_ROBOTY_GAME_STATE_H
