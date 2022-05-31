#include "server.h"

using namespace boost;

void Server::clientHandler(size_t index) {
    auto sock = socks[index];
    *sock << ServerMessage(server.getHelloMessage());
    bool joined = false;
    Player player;
    PlayerId id = 0;
    while (true) {
        try {
            ClientMessage m;
            *sock >> m;
            unique_lock l(mutex);
            if (messageType(m) == ClientMessageEnum::Join) {
                if (!joined && !game) {
                    player.name = std::get<JoinMessage>(m).getName();
                    player.address = sock->getAddress();
                    joined = true;
                    id = server.addPlayer(player);
                    remaining_players.try_count_down();
                    for (const auto& ptr : socks)
                        *ptr << ServerMessage(AcceptedPlayerMessage(id, player));
                }
            } else {
                if (game && joined) {
                    game->addPlayerMove(m, id);
                }
            }
        } catch (...) {

        }
    }
}

Server::Server(const ServerOptions& options) : server(options), provider(options.getPort()),
                                               remaining_players(options.getPlayersCount()) {
    thread listener([this]() {
        size_t index;
        while (true) {
            auto ptr = std::make_shared<TCPServerSockStream>(provider);
            unique_lock lock(mutex);
            socks.push_back(ptr);
            client_handlers.emplace_back(std::make_shared<thread>([this, &index]() {
                return clientHandler(index++);
            }));
            lock.release();
        }
    });

    uint16_t game_length = options.getGameLength();
    uint64_t turn_duration = options.getTurnDuration();
    uint8_t players_count = options.getPlayersCount();

    thread turn_manager([this, game_length, turn_duration, players_count]() {
        while (true) {
            remaining_players.wait();
            unique_lock lock(mutex);
            game = std::make_unique<GameState>(server);
            for (const auto &ptr : socks)
                *ptr << ServerMessage(GameStartedMessage(server.getPlayerMap()));
            lock.release();
            for (uint16_t i = 0; i < game_length; i++) {
                unique_lock l(mutex);
                game->updateTurn();
                for (const auto &ptr : socks)
                    *ptr << ServerMessage(TurnMessage(i, game->getEvents()));
                l.release();
                this_thread::sleep_for(chrono::milliseconds(turn_duration));
            }
            lock.lock();
            for (const auto &ptr : socks)
                *ptr << ServerMessage(GameEndedMessage(game->getScores()));
            remaining_players.reset(players_count);
            game.reset();
            server.clearPlayers();
            lock.release();
        }
    });

    turn_manager.join();
    for (const auto &thr : client_handlers)
        thr->join();
}
