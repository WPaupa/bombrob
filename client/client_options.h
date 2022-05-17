#ifndef BOMBOWE_ROBOTY_CLIENT_OPTIONS_H
#define BOMBOWE_ROBOTY_CLIENT_OPTIONS_H

#include <cstdint>
#include <string>
#include <optional>
#include <ostream>
#include <boost/asio.hpp>

class ClientOptions {
private:
    boost::asio::ip::udp::endpoint display_endpoint;
    std::string player_name;
    uint16_t port;
    boost::asio::ip::tcp::resolver::results_type server_endpoint;
public:
    ClientOptions(int argc, char **argv);
    friend std::ostream& operator<<(std::ostream &, const ClientOptions &);
};

#endif //BOMBOWE_ROBOTY_CLIENT_OPTIONS_H
