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

// Podobnie jak wyrażenia, klasy wiadomości
// mają składowe zgodne z tymi z treści,
// gettery dla wszystkich składowych, konstruktory
// bezargumentowe i ze składowych oraz
// operatory przesłania do i ze strumienia. Konwencja
// nazewnicza: klasa odpowiadająca wiadomości
// Type nazywa się TypeMessage.

class JoinMessage {
private:
    std::string name;
public:
    JoinMessage() = default;

    explicit JoinMessage(std::string name) : name(std::move(name)) {}

    friend SockStream &operator<<(SockStream &, const JoinMessage &);
    friend SockStream &operator>>(SockStream &, JoinMessage &);

    [[nodiscard]] std::string getName() const {
        return name;
    }
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
    MoveMessage() = default;

    explicit MoveMessage(enum Direction direction) : direction(direction) {}

    friend SockStream &operator<<(SockStream &, const MoveMessage &);
    friend SockStream &operator>>(SockStream &, MoveMessage &);

    [[nodiscard]] Direction getDirection() const {
        return direction;
    }
};

// Typ wiadomości od klienta
using ClientMessage = std::variant<JoinMessage, PlaceBombMessage, PlaceBlockMessage, MoveMessage>;

inline ClientMessageEnum messageType(const ClientMessage &message) {
    return std::visit([](auto &&v) {
        using mess_t = std::decay_t<typeof(v)>;
        if constexpr (std::is_same_v<mess_t, JoinMessage>)
            return ClientMessageEnum::Join;
        else if constexpr (std::is_same_v<mess_t, PlaceBombMessage>)
            return ClientMessageEnum::PlaceBomb;
        else if constexpr (std::is_same_v<mess_t, PlaceBlockMessage>)
            return ClientMessageEnum::PlaceBlock;
        else
            return ClientMessageEnum::Move;
    }, message);
}

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
    HelloMessage() : players_count(), size_x(), size_y(), game_length(),
                     explosion_radius(), bomb_timer() {}

    HelloMessage(std::string server_name, uint8_t players_count, uint16_t size_x, uint16_t size_y,
                 uint16_t game_length, uint16_t explosion_radius, uint16_t bomb_timer)
            : server_name(std::move(server_name)),
              players_count(players_count),
              size_x(size_x), size_y(size_y),
              game_length(game_length),
              explosion_radius(explosion_radius),
              bomb_timer(bomb_timer) {}

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
    AcceptedPlayerMessage() : id() {}

    AcceptedPlayerMessage(uint8_t id, Player player) : id(id), player(std::move(player)) {}

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

    explicit GameStartedMessage(std::map<PlayerId, Player> players) : players(std::move(players)) {}

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
    TurnMessage() : turn() {}

    TurnMessage(uint16_t turn, std::vector<Event> events) : turn(turn), events(std::move(events)) {}

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

    explicit GameEndedMessage(std::map<PlayerId, Score> scores) : scores(std::move(scores)) {}

    friend SockStream &operator<<(SockStream &, const GameEndedMessage &);
    friend SockStream &operator>>(SockStream &, GameEndedMessage &);

    [[nodiscard]] std::map<PlayerId, Score> getScores() const {
        return scores;
    }
};

// Typ wiadomości od serwera.
using ServerMessage = std::variant<HelloMessage, AcceptedPlayerMessage, GameStartedMessage, TurnMessage, GameEndedMessage>;

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
    LobbyMessage() : players_count(), size_x(), size_y(), game_length(),
                     explosion_radius(), bomb_timer() {}

    LobbyMessage(std::string server_name, uint8_t players_count, uint16_t size_x, uint16_t size_y,
                 uint16_t game_length, uint16_t explosion_radius, uint16_t bomb_timer,
                 std::map<PlayerId, Player> players) : server_name(std::move(server_name)),
                                                        players_count(players_count),
                                                        size_x(size_x), size_y(size_y),
                                                        game_length(game_length),
                                                        explosion_radius(explosion_radius),
                                                        bomb_timer(bomb_timer),
                                                        players(std::move(players)) {}

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
    GameMessage() : size_x(), size_y(), game_length(), turn() {}

    GameMessage(std::string server_name, uint16_t size_x, uint16_t size_y, uint16_t game_length,
                uint16_t turn, std::map<PlayerId, Player> &players,
                std::map<PlayerId, Position> player_positions,
                std::vector<Position> blocks, std::vector<Bomb> bombs,
                std::vector<Position> explosions,
                std::map<PlayerId, Score> scores) : server_name(std::move(server_name)), size_x(size_x),
                                                     size_y(size_y),
                                                     game_length(game_length), turn(turn),
                                                     players(players),
                                                     player_positions(std::move(player_positions)),
                                                     blocks(std::move(blocks)), bombs(std::move(bombs)),
                                                     explosions(std::move(explosions)), scores(std::move(scores)) {}

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
