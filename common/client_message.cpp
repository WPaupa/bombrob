#include "message.h"
#include "socketutils.h"

using namespace std;
using namespace boost::asio::ip;

tcp::socket &operator<<(tcp::socket &sock, const JoinMessage &message) {
    return sock << message.name;
}

tcp::socket &operator>>(tcp::socket &sock, JoinMessage &message) {
    return sock >> message.name;
}

tcp::socket &operator<<(tcp::socket &sock, [[maybe_unused]] const PlaceBombMessage &message) {
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, [[maybe_unused]] PlaceBombMessage &message) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, [[maybe_unused]] const PlaceBlockMessage &message) {
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, [[maybe_unused]] PlaceBlockMessage &message) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const MoveMessage &message) {
    return sock << message.direction;
}

tcp::socket &operator>>(tcp::socket &sock, MoveMessage &message) {
    return sock >> message.direction;
}

tcp::socket &operator<<(tcp::socket &sock, const ClientMessage &message) {
    auto type(static_cast<ClientMessageEnum>(message.index()));
    sock << type;
    std::visit([&sock](auto &v) {
        sock << v;
    }, message);
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, ClientMessage &message) {
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