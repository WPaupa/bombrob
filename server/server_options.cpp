#include "server_options.h"
#include <boost/program_options.hpp>
#include <iostream>

using namespace boost;
namespace po = boost::program_options;
using namespace std;

namespace boost {
    void validate(boost::any& v, const std::vector<std::string>& values,
                  std::optional<uint32_t>*, int) {
        using namespace boost::program_options;
        validators::check_first_occurrence(v);
        const std::string& s = validators::get_single_string(values);

        auto n = boost::lexical_cast<uint32_t>(s);
        v      = boost::any(std::make_optional<uint32_t>(n));
    }
} // namespace boost

ServerOptions::ServerOptions(int argc, char **argv) {

    po::options_description desc("Allowed options");
    desc.add_options()
            ("bomb-timer,b", po::value<uint16_t>(&bomb_timer)->required()->value_name("<u16>"))
            ("players-count,c", po::value<uint8_t>(&players_count)->required()->value_name("<u8>"))
            ("turn-duration,d", po::value<uint64_t>(&turn_duration)->required()->value_name("<u64, milisekundy>"))
            ("explosion-radius,e", po::value<uint16_t>(&explosion_radius)->required()->value_name("<u16>"))
            ("help,h", "Print help information")
            ("initial-blocks,k", po::value<uint16_t>(&initial_blocks)->required()->value_name("<u16>"))
            ("game-length,l", po::value<uint16_t>(&game_length)->required()->value_name("<u16>"))
            ("server-name,n", po::value<string>(&server_name)->required()->value_name("<String>"))
            ("port,p", po::value<uint16_t>(&port)->required()->value_name("<u16>"))
            ("seed,s", po::value<std::optional<uint32_t>>(&seed)->value_name("<u32>"))
            ("size-x,x", po::value<uint16_t>(&size_x)->value_name("<u16>"))
            ("size-y,y", po::value<uint16_t>(&size_y)->value_name("<u16>"))
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

ostream &operator<<(ostream &os, const ServerOptions& o) {
    return os << "Bomb timer: " << o.bomb_timer
              << "\nPlayers count: " << o.players_count
              << "\nTurn duration: " << o.turn_duration
              << "\nExplosion radius: " << o.explosion_radius
              << "\nInitial blocks: " << o.initial_blocks
              << "\nGame length: " << o.game_length
              << "\nServer name: " << o.server_name
              << "\nPort: " << o.port
              << "\nSeed (0 if empty): " << o.seed.value_or(0)
              << "\nSize (x): " << o.size_x
              << "\nSize (y): " << o.size_y;
}
