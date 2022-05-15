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