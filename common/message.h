#ifndef BOMBOWE_ROBOTY_MESSAGE_H
#define BOMBOWE_ROBOTY_MESSAGE_H
#include <string>
#include <boost/asio.hpp>
#include <utility>
#include <map>
#include <vector>
#include "types.h"
#include "event.h"

class MessageBase {
public:
    virtual void send(boost::asio::ip::tcp::socket socket) = 0;
};

enum class ClientMessageEnum : unsigned char {
    Join = 0,
    PlaceBomb = 1,
    PlaceBlock = 2,
    Move = 3,
};

class JoinMessage : MessageBase {
private:
    std::string name;
public:
    explicit JoinMessage(boost::asio::ip::tcp::socket socket);
    explicit JoinMessage(std::string &name) : name(name) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

class PlaceBombMessage : MessageBase {
public:
    explicit PlaceBombMessage(boost::asio::ip::tcp::socket socket);
    PlaceBombMessage() = default;
    void send(boost::asio::ip::tcp::socket socket) override;
};

class PlaceBlockMessage : MessageBase {
public:
    explicit PlaceBlockMessage(boost::asio::ip::tcp::socket socket);
    PlaceBlockMessage() = default;
    void send(boost::asio::ip::tcp::socket socket) override;
};

class MoveMessage : MessageBase {
private:
    Direction direction;
public:
    explicit MoveMessage(boost::asio::ip::tcp::socket socket);
    explicit MoveMessage(enum Direction direction) : direction(direction) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

using ClientMessage = Variant<JoinMessage, PlaceBombMessage, PlaceBlockMessage, MoveMessage>;

enum class ServerMessageEnum : unsigned char {
    Hello = 0,
    AcceptedPlayer = 1,
    GameStarted = 2,
    Turn = 3,
    GameEnded = 4,
};

class HelloMessage : MessageBase {
private:
    std::string server_name;
    uint8_t players_count;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    uint16_t explosion_radius;
    uint16_t bomb_timer;
public:
    explicit HelloMessage(boost::asio::ip::tcp::socket socket);
    HelloMessage(std::string &server_name, uint8_t players_count, uint16_t size_x, uint16_t size_y,
                 uint16_t game_length, uint16_t explosion_radius, uint16_t bomb_timer) : server_name(server_name),
                  players_count(players_count), size_x(size_x), size_y(size_y), game_length(game_length),
                  explosion_radius(explosion_radius), bomb_timer(bomb_timer) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

class AcceptedPlayerMessage : MessageBase {
private:
    uint8_t id;
    Player player;
public:
    explicit AcceptedPlayerMessage(boost::asio::ip::tcp::socket socket);
    AcceptedPlayerMessage(uint8_t id, Player &player) : id(id), player(player) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

class GameStartedMessage : MessageBase {
private:
    std::map<uint8_t, Player> players;
public:
    explicit GameStartedMessage(boost::asio::ip::tcp::socket socket);
    explicit GameStartedMessage(std::map<uint8_t, Player> &players) : players(players) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

class TurnMessage : MessageBase {
private:
    uint16_t turn;
    std::vector<Event> events;
public:
    explicit TurnMessage(boost::asio::ip::tcp::socket socket);
    TurnMessage(uint16_t turn, std::vector<Event> &events) : turn(turn), events(events) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

class GameEndedMessage : MessageBase {
private:
    std::map<PlayerId, Score> scores;
public:
    explicit GameEndedMessage(boost::asio::ip::tcp::socket socket);
    explicit GameEndedMessage(std::map<PlayerId, Score> &scores) : scores(scores) {}
    void send(boost::asio::ip::tcp::socket socket) override;
};

using ServerMessage = Variant<HelloMessage, AcceptedPlayerMessage, GameStartedMessage, TurnMessage, GameEndedMessage>;


#endif //BOMBOWE_ROBOTY_MESSAGE_H
