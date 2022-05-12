#ifndef BOMBOWE_ROBOTY_CLIENT_OPTIONS_H
#define BOMBOWE_ROBOTY_CLIENT_OPTIONS_H

#include <cstdint>
#include <string>
#include <optional>
#include <ostream>
#include <boost/asio.hpp>

class client_options {
private:
    std::string display_address;
    std::string player_name;
    uint16_t port;
    boost::asio::ip::tcp::endpoint server_endpoint;
public:
    client_options(int argc, char **argv);
    friend std::ostream& operator<<(std::ostream &, const client_options &);
};

#endif //BOMBOWE_ROBOTY_CLIENT_OPTIONS_H
