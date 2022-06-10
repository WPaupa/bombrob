#ifndef BOMBOWE_ROBOTY_SERVER_H
#define BOMBOWE_ROBOTY_SERVER_H

#include "state.h"
#include "../common/message.h"
#include <boost/thread.hpp>
#include <boost/thread/latch.hpp>
#include <list>

class Server {
private:
    std::shared_ptr<ServerState> server;
    std::unique_ptr<GameState> game;
    TCPServerSockStreamProvider provider;
    std::list<std::shared_ptr<boost::thread>> client_handlers;
    std::list<std::shared_ptr<TCPServerSockStream>> socks;
    std::list<bool> joined;
    boost::latch remaining_players;
    boost::mutex mutex;
    uint16_t game_length;
    uint64_t turn_duration;
    uint8_t players_count;

    void sendToAll(const ServerMessage &message);

    void clientHandler(std::list<std::shared_ptr<TCPServerSockStream>>::iterator sock,
            std::list<bool>::iterator join);

    void listener();

    void turn_manager(boost::exception_ptr &error);

public:
    explicit Server(const ServerOptions &options);
};


#endif //BOMBOWE_ROBOTY_SERVER_H
