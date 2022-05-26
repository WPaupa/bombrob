#include "message.h"
#include "socket_utils.h"

using namespace std;
using namespace boost::asio::ip;

SockStream &operator<<(SockStream &sock, const JoinMessage &message) {
    return sock << message.name;
}

SockStream &operator>>(SockStream &sock, JoinMessage &message) {
    return sock >> message.name;
}

SockStream &operator<<(SockStream &sock, [[maybe_unused]] const PlaceBombMessage &message) {
    return sock;
}

SockStream &operator>>(SockStream &sock, [[maybe_unused]] PlaceBombMessage &message) {
    return sock;
}

SockStream &operator<<(SockStream &sock, [[maybe_unused]] const PlaceBlockMessage &message) {
    return sock;
}

SockStream &operator>>(SockStream &sock, [[maybe_unused]] PlaceBlockMessage &message) {
    return sock;
}

SockStream &operator<<(SockStream &sock, const MoveMessage &message) {
    return sock << message.direction;
}

SockStream &operator>>(SockStream &sock, MoveMessage &message) {
    return sock >> message.direction;
}

SockStream &operator<<(SockStream &sock, const ClientMessage &message) {
    auto type(static_cast<ClientMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    return sock;
}

SockStream &operator>>(SockStream &sock, ClientMessage &message) {
    ClientMessageEnum type;
    sock >> type;
    switch (type) {
        case ClientMessageEnum::Join:
            message.emplace<JoinMessage>();
            break;
        case ClientMessageEnum::PlaceBomb:
            message.emplace<PlaceBombMessage>();
            break;
        case ClientMessageEnum::PlaceBlock:
            message.emplace<PlaceBlockMessage>();
            break;
        case ClientMessageEnum::Move:
            message.emplace<MoveMessage>();
            break;
    }
    std::visit([&sock](auto &v) {
        sock >> v;
    }, message);
    return sock;
}