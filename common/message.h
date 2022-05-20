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
    explicit JoinMessage(std::string name) : name(std::move(name)) {}
    friend SockStream &operator<<(SockStream &, const JoinMessage &);
    friend SockStream &operator>>(SockStream &, JoinMessage &);
};

class PlaceBombMessage {
public:
    PlaceBombMessage() = default;
    friend SockStream &operator<<(SockStream &, const PlaceBombMessage &);
    friend SockStream &operator>>(SockStream &, PlaceBombMessage &);
};

class PlaceBlockMessage {
public:
    PlaceBlockMessage() = default;
    friend SockStream &operator<<(SockStream &, const PlaceBlockMessage &);
    friend SockStream &operator>>(SockStream &, PlaceBlockMessage &);
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
    friend SockStream &operator<<(SockStream &, const MoveMessage &);
    friend SockStream &operator>>(SockStream &, MoveMessage &);
};

using ClientMessage = std::variant<JoinMessage, PlaceBombMessage, PlaceBlockMessage, MoveMessage>;

SockStream &operator<<(SockStream &, const ClientMessage &);
SockStream &operator>>(SockStream &, ClientMessage &);

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
    friend SockStream &operator<<(SockStream &, const HelloMessage &);
    friend SockStream &operator>>(SockStream &, HelloMessage &);

    [[nodiscard]] std::string getServerName() const {
        return server_name;
    }
    [[nodiscard]] uint8_t getPlayersCount() const {
        return players_count;
    }
    [[nodiscard]] uint16_t getSizeX() const {
        return size_x;
    }
    [[nodiscard]] uint16_t getSizeY() const {
        return size_y;
    }
    [[nodiscard]] uint16_t getGameLength() const {
        return game_length;
    }
    [[nodiscard]] uint16_t getExplosionRadius() const {
        return explosion_radius;
    }
    [[nodiscard]] uint16_t getBombTimer() const {
        return bomb_timer;
    }
};

class AcceptedPlayerMessage {
private:
    uint8_t id;
    Player player;
public:
    AcceptedPlayerMessage() = default;
    AcceptedPlayerMessage(uint8_t id, Player &player) : id(id), player(player) {}
    friend SockStream &operator<<(SockStream &, const AcceptedPlayerMessage &);
    friend SockStream &operator>>(SockStream &, AcceptedPlayerMessage &);

    [[nodiscard]] uint8_t getId() const {
        return id;
    }
    [[nodiscard]] Player getPlayer() const {
        return player;
    }
};

class GameStartedMessage {
private:
    std::map<PlayerId, Player> players;
public:
    GameStartedMessage() = default;
    explicit GameStartedMessage(std::map<PlayerId, Player> &players) : players(players) {}
    friend SockStream &operator<<(SockStream &, const GameStartedMessage &);
    friend SockStream &operator>>(SockStream &, GameStartedMessage &);
    [[nodiscard]] std::map<uint8_t, Player> getPlayers() {
        return players;
    }
};

class TurnMessage {
private:
    uint16_t turn;
    std::vector<Event> events;
public:
    TurnMessage() = default;
    TurnMessage(uint16_t turn, std::vector<Event> &events) : turn(turn), events(events) {}
    friend SockStream &operator<<(SockStream &, const TurnMessage &);
    friend SockStream &operator>>(SockStream &, TurnMessage &);

    [[nodiscard]] uint16_t getTurn() const {
        return turn;
    }
    [[nodiscard]] std::vector<Event> getEvents() const {
        return events;
    }
};

class GameEndedMessage {
private:
    std::map<PlayerId, Score> scores;
public:
    GameEndedMessage() = default;
    explicit GameEndedMessage(std::map<PlayerId, Score> &scores) : scores(scores) {}
    friend SockStream &operator<<(SockStream &, const GameEndedMessage &);
    friend SockStream &operator>>(SockStream &, GameEndedMessage &);
    [[nodiscard]] std::map<PlayerId, Score> getScores() const {
        return scores;
    }
};

using ServerMessage = std::variant<HelloMessage, 
                                   AcceptedPlayerMessage, GameStartedMessage, TurnMessage, GameEndedMessage>;

SockStream &operator<<(SockStream &, const ServerMessage &);
SockStream &operator>>(SockStream &, ServerMessage &);

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
    friend SockStream &operator<<(SockStream &, const LobbyMessage &);
    friend SockStream &operator>>(SockStream &, LobbyMessage &);
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
    friend SockStream &operator<<(SockStream &, const GameMessage &);
    friend SockStream &operator>>(SockStream &, GameMessage &);
};

using DrawMessage = std::variant<LobbyMessage, GameMessage>;

SockStream &operator<<(SockStream &, const DrawMessage &);
SockStream &operator>>(SockStream &, DrawMessage &);

enum class InputMessageEnum : uint8_t {
    PlaceBomb = 0,
    PlaceBlock = 1,
    Move = 2,
};

using InputMessage = std::variant<PlaceBombMessage, PlaceBlockMessage, MoveMessage>;

SockStream &operator<<(SockStream &, const InputMessage &);
SockStream &operator>>(SockStream &, InputMessage &);

#endif //BOMBOWE_ROBOTY_MESSAGE_H
