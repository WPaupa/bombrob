#include "state.h"

using namespace std;

GameState::GameState(ServerState &state) : state(state) {
    turn = 0;
    events = std::vector<Event>();

    for (PlayerId id = 0; id < state.playerCount(); id++) {
        Position position{static_cast<uint16_t>(random() % state.options.getSizeX()),
                          static_cast<uint16_t>(random() % state.options.getSizeY())};
        player_positions.insert({id, position});
        player_deaths.insert({id, 0});
        events.emplace_back(PlayerMovedEvent(id, position));
    }

    for (uint16_t i = 0; i < state.options.getInitialBlocks(); i++) {
        Position position{static_cast<uint16_t>(random() % state.options.getSizeX()),
                          static_cast<uint16_t>(random() % state.options.getSizeY())};
        blocks.insert(position);
        events.emplace_back(BlockPlacedEvent(position));
    }
}

void GameState::addPlayerMove(ClientMessage &message, ClientMessageEnum type, Player &player) {
    PlayerId id = 0;
    while (state.players[id].name != player.name || state.players[id].address != player.address) id++;
    player_moves.insert({id, {type, message}});
}

void GameState::explodeBombs(set<Position> &exploding_blocks, set<PlayerId> &exploding_players) {
    for (auto &[id, data] : bombs) {
        auto &pos = data.first;
        auto &timer = data.second;
        timer--;
        if (timer == 0) {
            vector<Position> current_blocks;
            vector<PlayerId> current_players;
            for (uint16_t i = 0; i <= state.options.getExplosionRadius(); i++) {
                Position next(pos, Direction::Right, i);
                if (next.x >= state.options.getSizeX())
                    break;
                for (auto [pid, ppos] : player_positions)
                    if (ppos == next) {
                        current_players.push_back(pid);
                        exploding_players.insert(pid);
                    }
                if (blocks.contains(next)) {
                    current_blocks.push_back(next);
                    exploding_blocks.insert(next);
                    break;
                }
            }
            for (uint16_t i = 1; i <= state.options.getExplosionRadius(); i++) {
                Position next(pos, Direction::Left, i);
                if (next.x >= state.options.getSizeX())
                    break;
                for (auto [pid, ppos] : player_positions)
                    if (ppos == next) {
                        current_players.push_back(pid);
                        exploding_players.insert(pid);
                    }
                if (blocks.contains(next)) {
                    current_blocks.push_back(next);
                    exploding_blocks.insert(next);
                    break;
                }
            }
            for (uint16_t i = 1; i <= state.options.getExplosionRadius(); i++) {
                Position next(pos, Direction::Up, i);
                if (next.y >= state.options.getSizeY())
                    break;
                for (auto [pid, ppos] : player_positions)
                    if (ppos == next) {
                        current_players.push_back(pid);
                        exploding_players.insert(pid);
                    }
                if (blocks.contains(next)) {
                    current_blocks.push_back(next);
                    exploding_blocks.insert(next);
                    break;
                }
            }
            for (uint16_t i = 1; i <= state.options.getExplosionRadius(); i++) {
                Position next(pos, Direction::Down, i);
                if (next.y >= state.options.getSizeY())
                    break;
                for (auto [pid, ppos] : player_positions)
                    if (ppos == next) {
                        current_players.push_back(pid);
                        exploding_players.insert(pid);
                    }
                if (blocks.contains(next)) {
                    current_blocks.push_back(next);
                    exploding_blocks.insert(next);
                    break;
                }
            }
            events.emplace_back(BombExplodedEvent(id, current_players, current_blocks));
        }
    }
}

void GameState::executePlayerMove(ClientMessage &message, ClientMessageEnum type, PlayerId id) {
    switch (type) {
        case ClientMessageEnum::Join:
            throw invalid_argument("Join message");
        case ClientMessageEnum::PlaceBomb: {
            pair<BombId, pair<Position, uint16_t>> bomb = {bombs.size(),
                                                           {player_positions[id], state.options.getBombTimer()}};
            events.emplace_back(BombPlacedEvent(bomb.first, bomb.second.first));
            bombs.insert(bomb);
            break;
        }
        case ClientMessageEnum::PlaceBlock:
            if (blocks.contains(player_positions[id]))
                break;
            blocks.insert(player_positions[id]);
            events.emplace_back(BlockPlacedEvent(player_positions[id]));
            break;
        case ClientMessageEnum::Move: {
            Position next(player_positions[id], get<MoveMessage>(message).getDirection(), 1);
            if (blocks.contains(next) ||
                next.x >= state.options.getSizeX() || next.y >= state.options.getSizeY())
                break;
            player_positions[id] = next;
            events.emplace_back(PlayerMovedEvent(id, next));
            break;
        }
    }
}

void GameState::updateTurn() {
    events = vector<Event>();
    set<Position> exploding_blocks;
    set<PlayerId> exploding_players;
    explodeBombs(exploding_blocks, exploding_players);
    for (auto block : exploding_blocks)
        blocks.erase(block);
    for (PlayerId id = 0; id < state.playerCount(); id++) {
        if (!exploding_players.contains(id)) {
            if (player_moves.contains(id)) {
                executePlayerMove(player_moves[id].second, player_moves[id].first, id);
            }
        } else {
            Position position{static_cast<uint16_t>(random() % state.options.getSizeX()),
                              static_cast<uint16_t>(random() % state.options.getSizeY())};
            player_positions[id] = position;
            events.emplace_back(PlayerMovedEvent(id, position));
        }
    }
    turn++;
}

std::vector<Event> &GameState::getEvents() {
    return events;
}

PlayerId ServerState::addPlayer(Player &player) {
    players.push_back(player);
    if (players.size() - 1 > UINT8_MAX)
        throw std::overflow_error("Player vector");
    return playerCount() - 1;
}

Player ServerState::getPlayer(PlayerId id) {
    return players[id];
}

PlayerId ServerState::playerCount() {
    return static_cast<PlayerId>(players.size());
}