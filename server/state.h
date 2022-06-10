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
    Random r;
    friend GameState;
public:
    explicit ServerState(ServerOptions options) : options(std::move(options)), r(options.getSeed()) {}

    PlayerId addPlayer(Player &player);

    Player getPlayer(PlayerId id);

    PlayerId playerCount();

    HelloMessage getHelloMessage();

    void clearPlayers();

    std::map<PlayerId, Player> getPlayerMap();

    Random &random();
};

class GameState {
private:
    std::shared_ptr<ServerState> state;
    uint16_t turn;
    std::vector<Event> events;
    std::map<PlayerId, Position> player_positions;
    std::map<BombId, std::pair<Position, uint16_t>> bombs;
    std::map<PlayerId, ClientMessage> player_moves;
    std::set<Position> blocks;
    std::map<PlayerId, Score> scores;
    std::vector<std::vector<Event>> turns;

    void explodeBombs(std::set<Position> &, std::set<PlayerId> &);

    void executePlayerMove(ClientMessage &message, PlayerId id);

public:
    explicit GameState(std::shared_ptr<ServerState> state);

    void addPlayerMove(ClientMessage &message, PlayerId id);

    void updateTurn();

    const std::vector<Event> &getEvents();

    const std::map<PlayerId, Score> &getScores();

    const std::vector<std::vector<Event>> &getTurns();
};


#endif //BOMBOWE_ROBOTY_GAME_STATE_H
