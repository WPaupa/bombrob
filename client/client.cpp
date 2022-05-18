#include "client.h"
#include <boost/thread.hpp>
#include <boost/thread/latch.hpp>
#include "../common/message.h"

#include <iostream>

using namespace boost;

Client::Client(ClientOptions &options)
    : server(options.getServerAddress()), display(options.getDisplayAddress(), options.getPort()), lobby(true)  {
    latch l(1);

    thread display_thread([this, &l](){
        InputMessage m;
        display >> m;
        l.count_down();
    });
    thread server_thread([this, &l](){
        // inicjujemy najpierw na pustą wiadomość;
        ServerMessage m = HelloMessage();
        while (!is_same_message<GameStartedMessage>(m))
            server >> m;
        if (l.try_count_down())
            lobby = false;
        //l.count_down();
    });

    l.wait();
    if (!lobby)
        throw std::runtime_error("Game started");
    server << ClientMessage(JoinMessage(options.getPlayerName()));
    ServerMessage m;
    server >> m;
    if (!is_same_message<HelloMessage>(m))
        throw std::runtime_error("Wrong message");
}