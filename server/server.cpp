#include "server.h"

using namespace boost;

// Metoda wysyłająca każdemu klientowi
// zamyka socketa w przypadku błędu
// (i to tyle, klient z zamkniętego
// socketa po sobie posprząta).
void Server::sendToAll(const ServerMessage &message) {
    for (const auto &ptr: socks)
        try {
            *ptr << message;
        } catch (...) {
            ptr->stop();
        }
}

// Metoda wywoływana jako wątek dla każdego klienta.
// Na początek dziedziczy sekcję krytyczną od metody
// listener. Najpierw musi wysłać informację o zaakceptowanych
// do tej pory graczach (lub turach, jeśli
// zaczęła się już gra), a potem w pętli
// akceptuje komunikaty od klienta i przekazuje
// do klasy obsługującej stan gry. W razie wyjątku
// sprząta i kończy działanie.
void Server::clientHandler(
        std::list<std::shared_ptr<TCPServerSockStream>>::iterator sock,
        std::list<bool>::iterator join) {
    try {
        **sock << ServerMessage(server->getHelloMessage());
        if (game) {
            **sock << ServerMessage(GameStartedMessage(server->getPlayerMap()));
            auto turns = static_cast<uint16_t>(game->getTurns().size());
            for (uint16_t i = 0; i < turns; i++)
                **sock << ServerMessage(TurnMessage(i, game->getTurns()[i]));
        } else {
            for (auto &&[id, player]: server->getPlayerMap())
                **sock << ServerMessage(AcceptedPlayerMessage(id, player));
        }
    } catch (...) {
        DEBUG("L player\n");
        mutex.unlock();
        return;
    }
    DEBUG("Listener releasing lock\n");
    mutex.unlock();
    Player player;
    player.address = (*sock)->getAddress();
    PlayerId id = 0;
    DEBUG("Waiting for message from %s\n", (*sock)->getAddress().c_str());
    bool mutex_locked = false;
    while (true) {
        try {
            ClientMessage m;
            **sock >> m;
            DEBUG("Acquiring lock...\n");
            mutex.lock();
            mutex_locked = true;
            DEBUG("Acquired!\n");
            if (messageType(m) == ClientMessageEnum::Join) {
                if (!*join && !game && server->playerCanJoin()) {
                    player.name = std::get<JoinMessage>(m).getName();
                    DEBUG("Adding new player: %s %s\n", player.name.c_str(),
                          player.address.c_str());
                    *join = true;
                    id = server->addPlayer(player);
                    remaining_players.try_count_down();
                    sendToAll(ServerMessage(AcceptedPlayerMessage(id, player)));
                }
            } else {
                if (game && *join) {
                    game->addPlayerMove(m, id);
                }
            }
            mutex_locked = false;
            mutex.unlock();
            DEBUG("Cleanly releasing lock\n");
        } catch (...) {
            DEBUG("Client %s disconnected\n", player.address.c_str());
            socks.erase(sock);
            joined.erase(join);
            if (mutex_locked) {
                mutex.unlock();
                DEBUG("Releasing lock through exception\n");
            }
            return;
        }
    }
}

// Metoda nasłuchująca na połączenia. Dodaje z każdym połączeniem
// nowy wątek i nowego socketa do listy.
void Server::listener() {
    while (true) {
        auto ptr = std::make_shared<TCPServerSockStream>(provider);
        DEBUG("Listener acquiring lock...\n");
        mutex.lock();
        DEBUG("Listener acquired!\n");
        socks.push_front(ptr);
        joined.push_front(false);
        client_handlers.emplace_back(std::make_shared<thread>([this]() {
            return clientHandler(socks.begin(), joined.begin());
        }));
    }
}

// Metoda zajmująca się turami. Na początek czeka
// na wystarczająco dużo graczy, a potem wysyła
// GameStarted, potem wysyła odpowiednio dużo
// tur, czekając odpowiednio długo, a potem
// wysyła GameEnded i ustawia stan wszystkich
// graczy na niedołączony.
void Server::turn_manager(exception_ptr &error) {
    try {
        while (true) {
            remaining_players.wait();
            DEBUG("Turn manager acquiring lock...\n");
            mutex.lock();
            DEBUG("Acuqired!\n");
            game = std::make_unique<GameState>(server);
            sendToAll(ServerMessage(GameStartedMessage(server->getPlayerMap())));
            DEBUG("Turn manager releasing lock\n");
            mutex.unlock();
            for (uint16_t i = 0; i < game_length; i++) {
                DEBUG("Turn manager acquiring inner lock...\n");
                mutex.lock();
                DEBUG("Acquired!\n");
                sendToAll(ServerMessage(TurnMessage(i, game->getEvents())));
                DEBUG("Turn manager releasing inner lock\n");
                mutex.unlock();
                this_thread::sleep_for(chrono::milliseconds(turn_duration));
                game->updateTurn();
            }
            DEBUG("Turn manager acquiring second lock...\n");
            mutex.lock();
            DEBUG("Acquired!\n");
            sendToAll(
                    ServerMessage(TurnMessage(game_length, game->getEvents())));
            sendToAll(ServerMessage(GameEndedMessage(game->getScores())));
            for (auto &&j: joined)
                j = false;
            remaining_players.reset(players_count);
            game.reset();
            server->clearPlayers();
            DEBUG("Turn manager releasing second lock\n");
            mutex.unlock();
        }
    } catch (...) {
        error = current_exception();
    }
}

// Tworzymy wątek nasłuchujący i wątek zajmujący się turami. W razie błędu
// wychodzimy z tymże błędem.
Server::Server(const ServerOptions &options) : server(std::make_shared<ServerState>(options)),
                                               provider(options.getPort()),
                                               remaining_players(options.getPlayersCount()) {
    thread listener(&Server::listener, this);

    game_length = options.getGameLength();
    turn_duration = options.getTurnDuration();
    players_count = options.getPlayersCount();

    exception_ptr error;

    thread turn_manager(&Server::turn_manager, this, error);

    turn_manager.join();
    for (const auto &ptr: socks)
        ptr->stop();
    for (const auto &thr: client_handlers)
        thr->join();
    rethrow_exception(error);
}
