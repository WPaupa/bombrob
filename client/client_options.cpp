#include "client_options.h"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>

using namespace boost;
namespace po = boost::program_options;
using namespace boost::asio::ip;
using namespace std;

namespace boost {
    void validate(boost::any& v, const std::vector<std::string>& values,
                  tcp::resolver::results_type*, int) {
        using namespace boost::program_options;
        validators::check_first_occurrence(v);
        const std::string& s = validators::get_single_string(values);

        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);

        size_t port_start = s.find_last_of(':');
        tcp::resolver::query query(s.substr(0, port_start), s.substr(port_start + 1));

        v = boost::any(resolver.resolve(query));
    }

    void validate(boost::any& v, const std::vector<std::string>& values,
                  udp::endpoint*, int) {
        using namespace boost::program_options;
        validators::check_first_occurrence(v);
        const std::string& s = validators::get_single_string(values);

        boost::asio::io_service io_service;
        udp::resolver resolver(io_service);

        size_t port_start = s.find_last_of(':');
        udp::resolver::query query(s.substr(0, port_start), s.substr(port_start + 1));
        udp::resolver::iterator iter = resolver.resolve(query);

        v = boost::any(iter->endpoint());
    }
} // namespace boost

ClientOptions::ClientOptions(int argc, char **argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("display-address,d", po::value<std::string>(&display_address)->required()
                                  ->value_name("<(nazwa hosta):(port) lub (IPv4):(port) lub (IPv6):(port)>"))
            ("help,h", "Print help information")
            ("player-name,n", po::value<string>(&player_name)->required()->value_name("<String>"))
            ("port,p", po::value<uint16_t>(&port)->required()->value_name("<u16>"))
            ("server-address,s", po::value<std::string>(&server_address)->required()
                                 ->value_name("<(nazwa hosta):(port) lub (IPv4):(port) lub (IPv6):(port)>"))
            ;

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
            throw std::exception();
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
