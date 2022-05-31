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
    explicit ServerState(ServerOptions options) : options(std::move(options)), random(options.getSeed()) {}

    PlayerId addPlayer(Player &player);

    Player getPlayer(PlayerId id);

    PlayerId playerCount();

    HelloMessage getHelloMessage();

    void clearPlayers();

    std::map<PlayerId, Player> getPlayerMap();
};

class GameState {
private:
    ServerState &state;
    uint16_t turn;
    std::vector<Event> events;
    std::map<PlayerId, Position> player_positions;
    std::map<BombId, std::pair<Position, uint16_t>> bombs;
    std::map<PlayerId, ClientMessage> player_moves;
    std::set<Position> blocks;
    std::map<PlayerId, Score> scores;

    void explodeBombs(std::set<Position> &, std::set<PlayerId> &);

    void executePlayerMove(ClientMessage &message, PlayerId id);

public:
    explicit GameState(ServerState &state);

    void addPlayerMove(ClientMessage &message, PlayerId id);

    void updateTurn();

    std::vector<Event> &getEvents();

    std::map<PlayerId, Score> getScores();
};


#endif //BOMBOWE_ROBOTY_GAME_STATE_H
