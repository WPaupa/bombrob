#include "message.h"
#include "socketutils.h"

using namespace std;
using namespace boost::asio::ip;

SockStream &operator<<(SockStream &sock, const HelloMessage &message) {
    return sock << message.server_name << message.players_count << message.size_x
           << message.size_y << message.game_length << message.explosion_radius
           << message.bomb_timer;
}

SockStream &operator>>(SockStream &sock, HelloMessage &message) {
    return sock >> message.server_name >> message.players_count >> message.size_x
           >> message.size_y >> message.game_length >> message.explosion_radius
           >> message.bomb_timer;
}

SockStream &operator<<(SockStream &sock, const AcceptedPlayerMessage &message) {
    return sock << message.id << message.player;
}

SockStream &operator>>(SockStream &sock, AcceptedPlayerMessage &message) {
    return sock >> message.id >> message.player;
}

SockStream &operator<<(SockStream &sock, const GameStartedMessage &message) {
    return sock << message.players;
}

SockStream &operator>>(SockStream &sock, GameStartedMessage &message) {
    return sock >> message.players;
}

SockStream &operator<<(SockStream &sock, const TurnMessage &message) {
    return sock << message.turn << message.events;
}

SockStream &operator>>(SockStream &sock, TurnMessage &message) {
    return sock >> message.turn >> message.events;
}

SockStream &operator<<(SockStream &sock, const GameEndedMessage &message) {
    return sock << message.scores;
}

SockStream &operator>>(SockStream &sock, GameEndedMessage &message) {
    return sock >> message.scores;
}

SockStream &operator<<(SockStream &sock, const ServerMessage &message) {
    auto type(static_cast<ServerMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    return sock;
}

SockStream &operator>>(SockStream &sock, ServerMessage &message) {
    ServerMessageEnum type;
    sock >> type;
    switch (type) {
        case ServerMessageEnum::Hello:
            message.emplace<HelloMessage>();
            break;
        case ServerMessageEnum::AcceptedPlayer:
            message.emplace<AcceptedPlayerMessage>();
            break;
        case ServerMessageEnum::GameStarted:
            message.emplace<GameStartedMessage>();
            break;
        case ServerMessageEnum::Turn:
            message.emplace<TurnMessage>();
            break;
        case ServerMessageEnum::GameEnded:
            message.emplace<GameEndedMessage>();
            break;
    }
    std::visit([&sock](auto &v) {
        sock >> v;
    }, message);
    return sock;
}