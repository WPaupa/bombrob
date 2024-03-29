#include "client_options.h"
#include "../common/exceptions.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>

namespace po = boost::program_options;
using namespace std;

// Przetwarzamy wszystkie opcje, a jeśli coś jest nie tak
// lub jeśli została użyta opcja -h, to wysyłamy wiadomość help.
ClientOptions::ClientOptions(int argc, char **argv) : port() {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("display-address,d", po::value<std::string>(&display_address)->required()
                    ->value_name("<(nazwa hosta):(port) lub (IPv4):(port) lub (IPv6):(port)>"))
            ("help,h", "Print help information")
            ("player-name,n", po::value<string>(&player_name)->required()->value_name("<String>"))
            ("port,p", po::value<uint16_t>(&port)->required()->value_name("<u16>"))
            ("server-address,s", po::value<std::string>(&server_address)->required()
                    ->value_name("<(nazwa hosta):(port) lub (IPv4):(port) lub (IPv6):(port)>"));

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
            throw Help();
        po::notify(vm);
    } catch (...) {
        cout << "Usage: " << argv[0] << " [options]\n";
        cout << desc;
        throw;
    }
}

std::ostream &operator<<(std::ostream &os, const ClientOptions &o) {
    return os << "Display address: " << o.display_address
              << "\nPlayer name: " << o.player_name
              << "\nPort: " << o.port
              << "\nServer address: " << o.server_address;
}
