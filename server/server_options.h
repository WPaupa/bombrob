#ifndef BOMBOWE_ROBOTY_SERVER_OPTIONS_H
#define BOMBOWE_ROBOTY_SERVER_OPTIONS_H

#include <cstdint>
#include <string>
#include <optional>
#include <ostream>

class server_options {
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
    server_options(int argc, char **argv);
    friend std::ostream& operator<<(std::ostream &, const server_options &);
};

#endif //BOMBOWE_ROBOTY_SERVER_OPTIONS_H
