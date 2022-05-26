#ifndef BOMBOWE_ROBOTY_SERVER_OPTIONS_H
#define BOMBOWE_ROBOTY_SERVER_OPTIONS_H

#include <cstdint>
#include <string>
#include <optional>
#include <ostream>

class ServerOptions {
private:
    uint16_t bomb_timer;
    uint8_t players_count;
    uint64_t turn_duration;
    uint16_t explosion_radius;
    uint16_t initial_blocks;
    uint16_t game_length;
    std::string server_name;
    uint16_t port;
    std::optional<uint32_t> seed;
    uint16_t size_x;
    uint16_t size_y;
public:
    ServerOptions(int argc, char **argv);

    friend std::ostream &operator<<(std::ostream &, const ServerOptions &);

    [[nodiscard]] uint16_t getBombTimer() const {
        return bomb_timer;
    }

    [[nodiscard]] uint8_t getPlayersCount() const {
        return players_count;
    }

    [[nodiscard]] uint64_t getTurnDuration() const {
        return turn_duration;
    }

    [[nodiscard]] uint16_t getExplosionRadius() const {
        return explosion_radius;
    }

    [[nodiscard]] uint16_t getInitialBlocks() const {
        return initial_blocks;
    }

    [[nodiscard]] uint16_t getGameLength() const {
        return game_length;
    }

    [[nodiscard]] std::string getServerName() const {
        return server_name;
    }

    [[nodiscard]] uint16_t getPort() const {
        return port;
    }

    [[nodiscard]] std::optional<uint32_t> getSeed() const {
        return seed;
    }

    [[nodiscard]] uint16_t getSizeX() const {
        return size_x;
    }

    [[nodiscard]] uint16_t getSizeY() const {
        return size_y;
    }
};

#endif //BOMBOWE_ROBOTY_SERVER_OPTIONS_H
