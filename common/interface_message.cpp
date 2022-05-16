#include "message.h"
#include "socketutils.h"

using namespace std;
using namespace boost::asio::ip;

tcp::socket &operator<<(tcp::socket &sock, const LobbyMessage &message) {
    return sock << message.server_name << message.players_count
                << message.size_x << message.size_y << message.game_length
                << message.explosion_radius << message.bomb_timer << message.players;
}

tcp::socket &operator>>(tcp::socket &sock, LobbyMessage &message) {
    return sock >> message.server_name >> message.players_count
                >> message.size_x >> message.size_y >> message.game_length
                >> message.explosion_radius >> message.bomb_timer >> message.players;
}

tcp::socket &operator<<(tcp::socket &sock, const GameMessage &message) {
    return sock << message.server_name << message.size_x << message.size_y
                << message.game_length << message.turn << message.players
                << message.player_positions << message.blocks << message.bombs
                << message.explosions << message.scores;
}

tcp::socket &operator>>(tcp::socket &sock, GameMessage &message) {
    return sock >> message.server_name >> message.size_x >> message.size_y
                >> message.game_length >> message.turn >> message.players
                >> message.player_positions >> message.blocks >> message.bombs
                >> message.explosions >> message.scores;
}

tcp::socket &operator<<(tcp::socket &sock, const DrawMessage &message) {
    auto type(static_cast<DrawMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, DrawMessage &message) {
    DrawMessageEnum type;
    sock >> type;
    switch (type) {
        case DrawMessageEnum::Lobby:
            message.emplace<LobbyMessage>();
            break;
        case DrawMessageEnum::Game:
            message.emplace<GameMessage>();
            break;
    }
    std::visit([&sock](auto &v) {
        sock >> v;
    }, message);
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const InputMessage &message) {
    auto type(static_cast<InputMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, InputMessage &message) {
    InputMessageEnum type;
    sock >> type;
    switch (type) {
        case InputMessageEnum::PlaceBomb:
            message.emplace<PlaceBombMessage>();
            break;
        case InputMessageEnum::PlaceBlock:
            message.emplace<PlaceBlockMessage>();
            break;
        case InputMessageEnum::Move:
            message.emplace<MoveMessage>();
            break;
    }
    std::visit([&sock](auto &v) {
        sock >> v;
    }, message);
    return sock;
}