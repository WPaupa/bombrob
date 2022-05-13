#include "message.h"

using namespace std;
using namespace boost::asio::ip;

JoinMessage::JoinMessage(tcp::socket socket) {

}

void JoinMessage::send(tcp::socket socket) {

}

PlaceBombMessage::PlaceBombMessage(boost::asio::ip::tcp::socket socket) {

}

void PlaceBombMessage::send(boost::asio::ip::tcp::socket socket) {

}

PlaceBlockMessage::PlaceBlockMessage(boost::asio::ip::tcp::socket socket) {

}

void PlaceBlockMessage::send(boost::asio::ip::tcp::socket socket) {

}

MoveMessage::MoveMessage(tcp::socket socket) {

}

void MoveMessage::send(tcp::socket socket) {

}

HelloMessage::HelloMessage(boost::asio::ip::tcp::socket socket) {

}

void HelloMessage::send(boost::asio::ip::tcp::socket socket) {

}

AcceptedPlayerMessage::AcceptedPlayerMessage(boost::asio::ip::tcp::socket socket) {

}

void AcceptedPlayerMessage::send(boost::asio::ip::tcp::socket socket) {

}

GameStartedMessage::GameStartedMessage(boost::asio::ip::tcp::socket socket) {

}

void GameStartedMessage::send(boost::asio::ip::tcp::socket socket) {

}

TurnMessage::TurnMessage(boost::asio::ip::tcp::socket socket) {

}

void TurnMessage::send(boost::asio::ip::tcp::socket socket) {

}

GameEndedMessage::GameEndedMessage(boost::asio::ip::tcp::socket socket) {

}

void GameEndedMessage::send(boost::asio::ip::tcp::socket socket) {

}
