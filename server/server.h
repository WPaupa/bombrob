#ifndef BOMBOWE_ROBOTY_SERVER_H
#define BOMBOWE_ROBOTY_SERVER_H
#include "state.h"
#include <boost/thread.hpp>
#include <boost/thread/latch.hpp>

class Server {
private:
    ServerState server;
    std::unique_ptr<GameState> game;
    TCPServerSockStreamProvider provider;
    std::vector<std::shared_ptr<boost::thread>> client_handlers;
    std::vector<std::shared_ptr<TCPServerSockStream>> socks;
    boost::latch remaining_players;
    boost::mutex mutex;
    void clientHandler(size_t index);
public:
    explicit Server(const ServerOptions &options);
};


#endif //BOMBOWE_ROBOTY_SERVER_H
