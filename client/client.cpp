#include "client.h"
#include <boost/thread.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/thread/latch.hpp>
#include "../common/message.h"
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
    sendToDisplay();
}

void Client::parseFromServer(TurnMessage &message) {
    turn = message.getTurn();
    explosions = std::vector<Position>();
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
                bool show_up = true, show_down = true,
                     show_left = true, show_right = true;
                Position initial = bomb_ids[b.getId()].position;
                for (uint16_t i = 0; i <= explosion_radius; i++) {
                    Position up(initial, Direction::Up, i);
                    if (up.y >= size_y)
                        show_up = false;
                    if (show_up)
                        explosions.push_back(up);
                    if (std::find(blocks.begin(), blocks.end(), up) != blocks.end())
                        show_up = false;
                    if (i == 0)
                        continue;
                    Position down(initial, Direction::Down, i);
                    if (down.y >= size_y)
                        show_down = false;
                    if (show_down)
                        explosions.push_back(down);
                    if (std::find(blocks.begin(), blocks.end(), down) != blocks.end())
                        show_down = false;
                    Position left(initial, Direction::Left, i);
                    if (left.x >= size_x)
                        show_left = false;
                    if (show_left)
                        explosions.push_back(left);
                    if (std::find(blocks.begin(), blocks.end(), left) != blocks.end())
                        show_left = false;
                    Position right(initial, Direction::Right, i);
                    if (right.x >= size_x)
                        show_right = false;
                    if (show_right)
                        explosions.push_back(right);
                    if (std::find(blocks.begin(), blocks.end(), right) != blocks.end())
                        show_right = false;
                }
                std::vector<Position>::iterator block_it;
                for (auto pos : b.getBlocksDestroyed())
                    if ((block_it = std::find(blocks.begin(), blocks.end(), pos)) != blocks.end())
                        blocks.erase(block_it);
                for (auto id : b.getRobotsDestroyed()) {
                    player_positions.erase(id);
                    scores[id]++;
                }
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
    sendToDisplay();
}

void Client::parseFromServer(GameEndedMessage &message) {
    scores = message.getScores();
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

Client::Client(ClientOptions &options)
    : server(options.getServerAddress()),
    display(options.getDisplayAddress(), options.getPort()), lobby(true),
    player_name(options.getPlayerName()) {

    latch l(1);
    boost::exception_ptr error{};
    thread display_thread([this, &error, &l](){
        try {
            InputMessage m;
            while (true) {
                fputs("Listening for message from display...\n", stderr);
                display >> m;
                std::visit([this](auto &&v) {
                    parseFromDisplay(v);
                }, m);
            }
        } catch (...) {
            fputs("Thread listening from display failed!\n", stderr);
            error = boost::current_exception();
            l.count_down();
        }
    });

    thread server_thread([this, &error, &l](){
        try {
            ServerMessage m;
            while (true) {
                fputs( "Listening for message from server...\n", stderr);
                server >> m;
                std::visit([this](auto &&v) {
                    parseFromServer(v);
                }, m);
            }
        } catch (...) {
            fputs("Thread listening from server failed!\n", stderr);
            error = boost::current_exception();
            l.count_down();
        }
    });
    l.wait();
    boost::rethrow_exception(error);
}