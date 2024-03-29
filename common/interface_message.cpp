#include "message.h"
#include "socket_utils.h"

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

SockStream &operator<<(SockStream &sock, const GameMessage &message) {
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

// Jako że DrawMessage odczytujemy z UDP,
// musimy nadać cały datagram po przesłaniu
// wiadomości do strumienia (metoda flushOut())
SockStream &operator<<(SockStream &sock, const DrawMessage &message) {
    auto type(static_cast<DrawMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    sock.flushOut();
    return sock;
}

// Znowu, jako że DrawMessage odczytujemy z UDP,
// przy błędnym typie wiadomości musimy
// odrzucić rozważany datagram (metoda flushIn()),
// a po odczytaniu całej wiadomości musimy
// upewnić się, że w datagramie nic więcej nie ma (taż metoda).
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