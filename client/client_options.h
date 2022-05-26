#ifndef BOMBOWE_ROBOTY_CLIENT_OPTIONS_H
#define BOMBOWE_ROBOTY_CLIENT_OPTIONS_H

#include <cstdint>
#include <string>
#include <optional>
#include <ostream>
#include <boost/asio.hpp>

// Klasa do parsowania opcji klienta, konstruowalna
// z opcji programu.
class ClientOptions {
private:
    std::string display_address;
    std::string player_name;
    uint16_t port;
    std::string server_address;
public:
    ClientOptions(int argc, char **argv);

    friend std::ostream &operator<<(std::ostream &, const ClientOptions &);

    [[nodiscard]] std::string getDisplayAddress() const {
        return display_address;
    }

    [[nodiscard]] std::string getPlayerName() const {
        return player_name;
    }

    [[nodiscard]] uint16_t getPort() const {
        return port;
    }

    [[nodiscard]] std::string getServerAddress() const {
        return server_address;
    }
};

#endif //BOMBOWE_ROBOTY_CLIENT_OPTIONS_H
