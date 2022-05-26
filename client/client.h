#ifndef BOMBOWE_ROBOTY_CLIENT_H
#define BOMBOWE_ROBOTY_CLIENT_H

#include "client_options.h"
#include "../common/sock_stream.h"
#include "../common/types.h"
#include "../common/message.h"
#include <map>
#include <set>
#include <vector>

class Client {
private:
    TCPSockStream server;
    UDPSockStream display;
    bool lobby;
    std::string server_name;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    uint16_t explosion_radius;
    uint16_t bomb_timer;
    std::map<PlayerId, Player> players;
    std::map<PlayerId, Position> player_positions;
    std::vector<Position> blocks;
    std::map<BombId, Bomb> bomb_ids;
    std::vector<Bomb> bombs;
    std::vector<Position> explosions;
    std::map<PlayerId, Score> scores;
    uint8_t players_count;
    uint16_t turn;
    std::string player_name;

    void parseFromServer(HelloMessage &message);
    void parseFromServer(AcceptedPlayerMessage &message);
    void parseFromServer(GameStartedMessage &message);
    void parseFromServer(TurnMessage &message);
    void parseFromServer(GameEndedMessage &message);

    template<typename T>
    void parseFromDisplay(T &message);

    void sendToDisplay();

public:
    explicit Client(ClientOptions &options);
};


#endif //BOMBOWE_ROBOTY_CLIENT_H
