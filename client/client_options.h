#ifndef BOMBOWE_ROBOTY_CLIENT_OPTIONS_H
#define BOMBOWE_ROBOTY_CLIENT_OPTIONS_H

#include <cstdint>
#include <string>
#include <optional>
#include <ostream>
#include <boost/asio.hpp>

class ClientOptions {
private:
    std::string display_address;
    std::string player_name;
    uint16_t port;
    boost::asio::ip::tcp::endpoint server_endpoint;
public:
    ClientOptions(int argc, char **argv);
    friend std::ostream& operator<<(std::ostream &, const ClientOptions &);
};

#endif //BOMBOWE_ROBOTY_CLIENT_OPTIONS_H
