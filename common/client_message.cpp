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

tcp::socket &operator<<(tcp::socket &sock, const PlaceBombMessage &message) {
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, PlaceBombMessage &message) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const PlaceBlockMessage &message) {
    return sock;
}

tcp::socket &operator>>(tcp::socket &sock, PlaceBlockMessage &message) {
    return sock;
}

tcp::socket &operator<<(tcp::socket &sock, const MoveMessage &message) {
    return sock << message.direction;
}

tcp::socket &operator>>(tcp::socket &sock, MoveMessage &message) {
    return sock >> message.direction;
}
