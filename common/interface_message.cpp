#include "message.h"

using namespace std;
using namespace boost::asio::ip;

tcp::socket &operator>>(tcp::socket &sock, const LobbyMessage &message) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, LobbyMessage &message) {
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, const GameMessage &message) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, GameMessage &message) {
    return sock;
}