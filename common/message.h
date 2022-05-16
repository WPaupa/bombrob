#ifndef BOMBOWE_ROBOTY_MESSAGE_H
#define BOMBOWE_ROBOTY_MESSAGE_H
#include <string>
#include <boost/asio.hpp>
#include <utility>
#include <map>
#include <vector>
#include "types.h"
#include "event.h"

enum class ClientMessageEnum : uint8_t {
    Join = 0,
    PlaceBomb = 1,
    PlaceBlock = 2,
    Move = 3,
};

class JoinMessage {
private:
    std::string name;
public:
    JoinMessage() = default;
    explicit JoinMessage(std::string &name) : name(name) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const JoinMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, JoinMessage &);
};

class PlaceBombMessage {
public:
    PlaceBombMessage() = default;
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const PlaceBombMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, PlaceBombMessage &);
};

class PlaceBlockMessage {
public:
    PlaceBlockMessage() = default;
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const PlaceBlockMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, PlaceBlockMessage &);
};

class MoveMessage {
private:
    Direction direction;
public:
    Direction getDirection() {
        return direction;
    }
    MoveMessage() = default;
    explicit MoveMessage(enum Direction direction) : direction(direction) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const MoveMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, MoveMessage &);
};

using ClientMessage = std::variant<JoinMessage, PlaceBombMessage, PlaceBlockMessage, MoveMessage>;

boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const ClientMessage &);
boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, ClientMessage &);

enum class ServerMessageEnum : uint8_t {
    Hello = 0,
    AcceptedPlayer = 1,
    GameStarted = 2,
    Turn = 3,
    GameEnded = 4,
};

class HelloMessage {
private:
    std::string server_name;
    uint8_t players_count;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    uint16_t explosion_radius;
    uint16_t bomb_timer;
public:
    HelloMessage() = default;
    HelloMessage(std::string &server_name, uint8_t players_count, uint16_t size_x, uint16_t size_y,
                 uint16_t game_length, uint16_t explosion_radius, uint16_t bomb_timer) : server_name(server_name),
                  players_count(players_count), size_x(size_x), size_y(size_y), game_length(game_length),
                  explosion_radius(explosion_radius), bomb_timer(bomb_timer) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const HelloMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, HelloMessage &);
};

class AcceptedPlayerMessage {
private:
    uint8_t id;
    Player player;
public:
    AcceptedPlayerMessage() = default;
    AcceptedPlayerMessage(uint8_t id, Player &player) : id(id), player(player) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const AcceptedPlayerMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, AcceptedPlayerMessage &);
};

class GameStartedMessage {
private:
    std::map<uint8_t, Player> players;
public:
    GameStartedMessage() = default;
    explicit GameStartedMessage(std::map<uint8_t, Player> &players) : players(players) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const GameStartedMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, GameStartedMessage &);
};

class TurnMessage {
private:
    uint16_t turn;
    std::vector<Event> events;
public:
    TurnMessage() = default;
    TurnMessage(uint16_t turn, std::vector<Event> &events) : turn(turn), events(events) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const TurnMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, TurnMessage &);
};

class GameEndedMessage {
private:
    std::map<PlayerId, Score> scores;
public:
    GameEndedMessage() = default;
    explicit GameEndedMessage(std::map<PlayerId, Score> &scores) : scores(scores) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const GameEndedMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, GameEndedMessage &);
};

using ServerMessage = std::variant<HelloMessage, 
                                   AcceptedPlayerMessage, GameStartedMessage, TurnMessage, GameEndedMessage>;

boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const ServerMessage &);
boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, ServerMessage &);

enum class DrawMessageEnum : uint8_t {
    Lobby = 0,
    Game = 1,
};

class LobbyMessage {
private:
    std::string server_name;
    uint8_t players_count;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    uint16_t explosion_radius;
    uint16_t bomb_timer;
    std::map<PlayerId, Player> players;
public:
    LobbyMessage() = default;
    LobbyMessage(std::string &server_name, uint8_t players_count, uint16_t size_x,
                 uint16_t size_y, uint16_t game_length, uint16_t explosion_radius,
                 uint16_t bomb_timer, std::map<PlayerId, Player> &players) :
                 server_name(server_name), players_count(players_count),
                 size_x(size_x), size_y(size_y), game_length(game_length),
                 explosion_radius(explosion_radius), bomb_timer(bomb_timer), players(players) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const LobbyMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, LobbyMessage &);
};

class GameMessage {
private:
    std::string server_name;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    uint16_t turn;
    std::map<PlayerId, Player> players;
    std::map<PlayerId, Position> player_positions;
    std::vector<Position> blocks;
    std::vector<Bomb> bombs;
    std::vector<Position> explosions;
    std::map<PlayerId, Score> scores;
public:
    GameMessage() = default;
    GameMessage(std::string &server_name, uint16_t size_x, uint16_t size_y,
                uint16_t game_length, uint16_t turn, std::map<PlayerId, Player> &players,
                std::map<PlayerId, Position> &player_positions,
                std::vector<Position> &blocks, std::vector<Bomb> &bombs,
                std::vector<Position> &explosions, std::map<PlayerId, Score> &scores) :
                server_name(server_name), size_x(size_x), size_y(size_y),
                game_length(game_length), turn(turn), players(players),
                player_positions(player_positions), blocks(blocks), bombs(bombs),
                explosions(explosions), scores(scores) {}
    friend boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const GameMessage &);
    friend boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, GameMessage &);
};

using DrawMessage = std::variant<LobbyMessage, GameMessage>;

boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const DrawMessage &);
boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, DrawMessage &);

enum class InputMessageEnum : uint8_t {
    PlaceBomb = 0,
    PlaceBlock = 1,
    Move = 2,
};

using InputMessage = std::variant<PlaceBombMessage, PlaceBlockMessage, MoveMessage>;

boost::asio::ip::tcp::socket &operator<<(boost::asio::ip::tcp::socket &, const InputMessage &);
boost::asio::ip::tcp::socket &operator>>(boost::asio::ip::tcp::socket &, InputMessage &);

#endif //BOMBOWE_ROBOTY_MESSAGE_H
