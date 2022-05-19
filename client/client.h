#ifndef BOMBOWE_ROBOTY_CLIENT_H
#define BOMBOWE_ROBOTY_CLIENT_H
#include "client_options.h"
#include "../common/sockstream.h"
#include "../common/types.h"
#include <map>
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
    std::vector<Bomb> bombs;
    std::vector<Position> explosions;
    std::map<PlayerId, Score> scores;
public:
    explicit Client(ClientOptions &options);
};


#endif //BOMBOWE_ROBOTY_CLIENT_H
