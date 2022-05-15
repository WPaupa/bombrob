#include "message.h"
#include "socketutils.h"

using namespace std;
using namespace boost::asio::ip;

tcp::socket &operator<<(tcp::socket &sock, const HelloMessage &message) {
    return sock << message.server_name << message.players_count << message.size_x
           << message.size_y << message.game_length << message.explosion_radius
           << message.bomb_timer;
}

tcp::socket &operator>>(tcp::socket &sock, HelloMessage &message) {
    return sock >> message.server_name >> message.players_count >> message.size_x
           >> message.size_y >> message.game_length >> message.explosion_radius
           >> message.bomb_timer;
}

tcp::socket &operator<<(tcp::socket &sock, const AcceptedPlayerMessage &message) {
    return sock << message.id << message.player;
}

tcp::socket &operator>>(tcp::socket &sock, AcceptedPlayerMessage &message) {
    return sock >> message.id >> message.player;
}

tcp::socket &operator<<(tcp::socket &sock, const GameStartedMessage &message) {
    return sock << message.players;
}

tcp::socket &operator>>(tcp::socket &sock, GameStartedMessage &message) {
    return sock >> message.players;
}

tcp::socket &operator<<(tcp::socket &sock, const TurnMessage &message) {
    return sock << message.turn << message.events;
}

tcp::socket &operator>>(tcp::socket &sock, TurnMessage &message) {
    return sock >> message.turn >> message.events;
}

tcp::socket &operator<<(tcp::socket &sock, const GameEndedMessage &message) {
    return sock << message.scores;
}

tcp::socket &operator>>(tcp::socket &sock, GameEndedMessage &message) {
    return sock >> message.scores;
}

tcp::socket &operator<<(tcp::socket &sock, const ServerMessage &message) {
    auto type(static_cast<ServerMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, ServerMessage &message) {
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