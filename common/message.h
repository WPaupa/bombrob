#ifndef BOMBOWE_ROBOTY_MESSAGE_H
#define BOMBOWE_ROBOTY_MESSAGE_H
#include <string>
#include <boost/asio.hpp>
#include <utility>
#include <map>
#include <vector>
#include "direction.h"

class message {
public:
    virtual void send(boost::asio::ip::tcp::socket socket) = 0;
};

enum class client_message : unsigned char {
    Join = 0,
    PlaceBomb = 1,
    PlaceBlock = 2,
    Move = 3,
};

class join_message : message {
private:
    std::string name;
public:
    explicit join_message(boost::asio::ip::tcp::socket socket);
    explicit join_message(std::string &name) : name(name) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

class direction_message : message {
private:
    direction direction;
public:
    explicit direction_message(boost::asio::ip::tcp::socket socket);
    explicit direction_message(enum direction direction) : direction(direction) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

enum class server_message : unsigned char {
    Hello = 0, /*
        server_name: String,
            players_count: u8,
                size_x: u16,
                size_y: u16,
                game_length: u16,
                explosion_radius: u16,
                bomb_timer: u16,
    */
    AcceptedPlayer = 1, /*
        id: PlayerId,
            player: Player,
    */
    GameStarted = 2, /*
        players: Map<PlayerId, Player>,
    */
    Turn = 3, /*
        turn: u16,
            events: List<Event>,
    */
    GameEnded = 4, /*
        scores: Map<PlayerId, Score>,
    */
};

class hello_message : message {
private:
    std::string server_name;
    uint8_t players_count;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    uint16_t explosion_radius;
    uint16_t bomb_timer;
public:
    explicit hello_message(boost::asio::ip::tcp::socket socket);
    hello_message(std::string &server_name, uint8_t players_count, uint16_t size_x, uint16_t size_y,
                  uint16_t game_length, uint16_t explosion_radius, uint16_t bomb_timer) : server_name(server_name),
                  players_count(players_count), size_x(size_x), size_y(size_y), game_length(game_length),
                  explosion_radius(explosion_radius), bomb_timer(bomb_timer) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

struct player {
    std::string name;
    std::string address;
};

class accepted_player_message : message {
private:
    uint8_t id;
    player player;
public:
    explicit accepted_player_message(boost::asio::ip::tcp::socket socket);
    accepted_player_message(uint8_t id, struct player &player) : id(id), player(player) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

class game_started_message : message {
private:
    std::map<uint8_t, player> players;
public:
    explicit game_started_message(boost::asio::ip::tcp::socket socket);
    explicit game_started_message(std::map<uint8_t, player> &players) : players(players) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

class turn_message : message {
private:
    std::vector<>
};




#endif //BOMBOWE_ROBOTY_MESSAGE_H
