#include "client.h"
#include <boost/thread.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/thread/latch.hpp>
#include <algorithm>

#include <iostream>

using namespace boost;

void Client::parseFromServer(HelloMessage &message) {
    server_name = message.getServerName();
    players_count = message.getPlayersCount();
    size_x = message.getSizeX();
    size_y = message.getSizeY();
    game_length = message.getGameLength();
    explosion_radius = message.getExplosionRadius();
    bomb_timer = message.getBombTimer();
    sendToDisplay();
}

void Client::parseFromServer(AcceptedPlayerMessage &message) {
    players.insert({message.getId(), message.getPlayer()});
    sendToDisplay();
}

void Client::parseFromServer(GameStartedMessage &message) {
    players = message.getPlayers();
    scores = std::map<PlayerId, Score>();
    for (auto &&[id, player] : players) {
        scores[id] = 0;
        player_positions[id] = {0, 0};
    }
    bombs = std::vector<Bomb>();
    bomb_ids = std::map<BombId, Bomb>();
    blocks = std::vector<Position>();
    lobby = false;
}

void Client::parseFromServer(TurnMessage &message) {
    turn = message.getTurn();
    auto explosion_set = std::set<Position>();
    std::set<PlayerId> robots_destroyed;
    std::set<Position> blocks_destroyed;
    for (auto &[id, bomb] : bomb_ids)
        bomb.timer--;
    for (auto &bomb : bombs)
        bomb.timer--;
    for (const Event &event : message.getEvents()) {
        switch (eventType(event)) {
            case EventEnum::BombPlaced: {
                auto b = get<BombPlacedEvent>(event);
                bomb_ids.insert({b.getId(), {b.getPosition(), bomb_timer}});
                bombs.emplace_back(b.getPosition(), bomb_timer);
                break;
            }
            case EventEnum::BombExploded: {
                auto b = get<BombExplodedEvent>(event);
                Position initial = bomb_ids[b.getId()].position;
                bool show[DIRECTION_MAX + 1];
                std::fill(show, show + DIRECTION_MAX + 1, true);
                for (uint16_t i = 0; i <= explosion_radius; i++) {
                    auto check = [&initial, &i, this, &explosion_set, &show](Direction d) {
                        Position next(initial, d, i);
                        bool *show_current = show + static_cast<uint8_t>(d);
                        if (next.x >= size_x || next.y >= size_y)
                            *show_current = false;
                        if (*show_current)
                            explosion_set.insert(next);
                        if (std::find(blocks.begin(), blocks.end(), next) != blocks.end())
                            *show_current = false;
                    };

                    check(Direction::Up);
                    check(Direction::Down);
                    check(Direction::Left);
                    check(Direction::Right);
                }
                for (auto pos : b.getBlocksDestroyed())
                    blocks_destroyed.insert(pos);
                for (auto id : b.getRobotsDestroyed())
                    robots_destroyed.insert(id);
                auto bomb_it = std::find(bombs.begin(), bombs.end(), bomb_ids[b.getId()]);
                if (bomb_it != bombs.end())
                    bombs.erase(bomb_it);
                bomb_ids.erase(b.getId());
                break;
            }
            case EventEnum::PlayerMoved: {
                auto p = get<PlayerMovedEvent>(event);
                player_positions[p.getId()] = p.getPosition();
                break;
            }
            case EventEnum::BlockPlaced: {
                auto b = get<BlockPlacedEvent>(event);
                blocks.push_back(b.getPosition());
                break;
            }
        }
    }

    explosions = std::vector<Position>(explosion_set.begin(), explosion_set.end());
    for (auto id : robots_destroyed)
        scores[id]++;
    std::vector<Position>::iterator block_it;
    for (auto pos : blocks_destroyed)
        if ((block_it = std::find(blocks.begin(), blocks.end(), pos)) != blocks.end())
            blocks.erase(block_it);

    sendToDisplay();
}

void Client::parseFromServer([[maybe_unused]] GameEndedMessage &message) {
    players = std::map<PlayerId, Player>();
    player_positions = std::map<PlayerId, Position>();
    lobby = true;
    sendToDisplay();
}

template<typename T>
void Client::parseFromDisplay(T &message) {
    if (lobby)
        server << ClientMessage(JoinMessage(player_name));
    else
        server << ClientMessage(message);
}

void Client::sendToDisplay() {
    if (lobby)
        display << DrawMessage(LobbyMessage(server_name, players_count, size_x,
                                            size_y, game_length, explosion_radius,
                                            bomb_timer, players));
    else
        display << DrawMessage(GameMessage(server_name, size_x, size_y, game_length,
                                           turn, players, player_positions,
                                           blocks, bombs, explosions, scores));
}

Client::Client(ClientOptions &options) : server(options.getServerAddress()),
                                         display(options.getDisplayAddress(),
                                                 options.getPort()), lobby(true),
                                         size_x(), size_y(), game_length(),
                                         explosion_radius(), bomb_timer(),
                                         players_count(), turn(),
                                         player_name(options.getPlayerName()) {

    latch l(1);
    bool ret = false;
    boost::exception_ptr error{};
    thread display_thread([this, &error, &l, &ret]() {
        try {
            InputMessage m;
            while (true) {
                fputs("Listening for message from display...\n", stderr);
                try {
                    display >> m;
                    std::visit([this](auto &&v) {
                        parseFromDisplay(v);
                    }, m);
                } catch (WrongMessage &e) {
                    fprintf(stderr, "Wrong message received from display: %s\n", e.what());
                }
            }
        } catch (...) {
            if (ret)
                return;
            ret = true;
            fputs("Thread listening from display failed!\n", stderr);
            error = boost::current_exception();
            l.count_down();
        }
    });

    thread server_thread([this, &error, &l, &ret]() {
        try {
            ServerMessage m;
            while (true) {
                fputs("Listening for message from server...\n", stderr);
                server >> m;
                std::visit([this](auto &&v) {
                    parseFromServer(v);
                }, m);
            }
        } catch (...) {
            if (ret)
                return;
            ret = true;
            fputs("Thread listening from server failed!\n", stderr);
            error = boost::current_exception();
            l.count_down();
        }
    });

    l.wait();
    server.stop();
    display.stop();
    server_thread.join();
    display_thread.join();
    boost::rethrow_exception(error);
}
