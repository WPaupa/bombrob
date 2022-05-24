#include "message.h"
#include "socketutils.h"

using namespace std;
using namespace boost::asio::ip;

SockStream &operator<<(SockStream &sock, const LobbyMessage &message) {
    return sock << message.server_name << message.players_count
                << message.size_x << message.size_y << message.game_length
                << message.explosion_radius << message.bomb_timer << message.players;
}

SockStream &operator>>(SockStream &sock, LobbyMessage &message) {
    return sock >> message.server_name >> message.players_count
                >> message.size_x >> message.size_y >> message.game_length
                >> message.explosion_radius >> message.bomb_timer >> message.players;
}

#include <iostream>

SockStream &operator<<(SockStream &sock, const GameMessage &message) {
    std::cerr << "Sending GameMessage: ";
    std::cerr << message.server_name << " " << message.size_x << " " << message.size_y << " "
              << message.game_length << " " << message.turn << " " << message.players.size() << " (";
    for (auto &&[k, v] : message.players)
        std::cerr << "[" << (int)k << " " << v.address << " " << v.name << "]";
    std::cerr << ") " << message.player_positions.size() << " (";
    for (auto &&[k, v] : message.player_positions)
        std::cerr << "[" << (int)k << " " << v.x << " " << v.y << "]";
    std::cerr << ") " << message.blocks.size() << " (";
    for (auto k : message.blocks)
        std::cerr << "[" << k.x << " " << k.y << "]";
    std::cerr << ") " << message.bombs.size() << " (";
    for (auto k : message.bombs)
        std::cerr << "[" << k.position.x << " " << k.position.y << " " << k.timer << "]";
    std::cerr << ") " << message.explosions.size() << " (";
    for (auto k : message.explosions)
        std::cerr << "[" << k.x << " " << k.y << "]";
    std::cerr << ") " << message.scores.size() << " (";
    for (auto &&[k, v] : message.scores)
        std::cerr << "[" << (int)k << " " << v << "]";
    std::cerr<< ")\n";
    return sock << message.server_name << message.size_x << message.size_y
                << message.game_length << message.turn << message.players
                << message.player_positions << message.blocks << message.bombs
                << message.explosions << message.scores;
}

SockStream &operator>>(SockStream &sock, GameMessage &message) {
    return sock >> message.server_name >> message.size_x >> message.size_y
                >> message.game_length >> message.turn >> message.players
                >> message.player_positions >> message.blocks >> message.bombs
                >> message.explosions >> message.scores;
}

SockStream &operator<<(SockStream &sock, const DrawMessage &message) {
    auto type(static_cast<DrawMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    sock.flushOut();
    return sock;
}

SockStream &operator>>(SockStream &sock, DrawMessage &message) {
    DrawMessageEnum type;
    sock >> type;
    switch (type) {
        case DrawMessageEnum::Lobby:
            message.emplace<LobbyMessage>();
            break;
        case DrawMessageEnum::Game:
            message.emplace<GameMessage>();
            break;
        default:
            try {
                sock.flushIn();
            } catch (WrongMessage &m) {}
            throw WrongMessage("Message type");
    }
    std::visit([&sock](auto &v) {
        sock >> v;
    }, message);
    sock.flushIn();
    return sock;
}

SockStream &operator<<(SockStream &sock, const InputMessage &message) {
    auto type(static_cast<InputMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    sock.flushOut();
    return sock;
}

SockStream &operator>>(SockStream &sock, InputMessage &message) {
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
        default:
            try {
                sock.flushIn();
            } catch (WrongMessage &m) {}
            throw WrongMessage("Message type");
    }
    std::visit([&sock](auto &v) {
        sock >> v;
    }, message);
    sock.flushIn();
    return sock;
}