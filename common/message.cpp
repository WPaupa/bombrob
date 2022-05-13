#include "message.h"

using namespace std;
using namespace boost::asio::ip;

join_message::join_message(tcp::socket socket) {

}

void join_message::send(tcp::socket socket) {

}

direction_message::direction_message(tcp::socket socket) {

}

void direction_message::send(tcp::socket socket) {

}

hello_message::hello_message(boost::asio::ip::tcp::socket socket) {

}

void hello_message::send(boost::asio::ip::tcp::socket socket) {

}

accepted_player_message::accepted_player_message(boost::asio::ip::tcp::socket socket) {

}

void accepted_player_message::send(boost::asio::ip::tcp::socket socket) {

}

game_started_message::game_started_message(boost::asio::ip::tcp::socket socket) {

}

void game_started_message::send(boost::asio::ip::tcp::socket socket) {

}
