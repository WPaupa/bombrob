#include "server_options.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/latch.hpp>
#include "../common/sock_stream.h"
#include "../common/socket_utils.h"
#include "../common/message.h"
#include <chrono>
#include <thread>


using namespace std;

class Work {
public:
    uint16_t bomb_timer = 1;
    uint8_t players_count = 2;
    uint64_t turn_duration = 300;
    uint16_t explosion_radius = 0;
    uint16_t initial_blocks = 0;
    uint16_t game_length = 0;
    string server_name = "Pong";
    uint16_t port;
    uint16_t size_x = 10;
    uint16_t size_y = 6;
    Position posb[2] = {{0, 3},
                        {9, 3}}, posa[2] = {{0, 3},
                                            {9, 3}};
    Position pball = {0, 3};
    Direction vhball = Direction::Right;
    Direction vvball = Direction::Up;
    shared_ptr<TCPSockStream> socks[2];
    boost::latch l{2};

    explicit Work(uint16_t nport) : port(nport) {
        auto work = [this](uint8_t n) {
            puts("DUPA");
            socks[n] = make_shared<TCPSockStream>(port + n);
            puts("ZUPA");
            *socks[n] << ServerMessage(HelloMessage(server_name, players_count, size_x, size_y, game_length, explosion_radius, bomb_timer));
            *socks[n] << ServerMessage(AcceptedPlayerMessage(0, {"Player 1", "[::1]:10"}));
            *socks[n] << ServerMessage(AcceptedPlayerMessage(1, {"Player 2", "[::1]:11"}));
            ClientMessage m;
            *socks[n] >> m;
            *socks[n] << ServerMessage(GameStartedMessage(std::map<PlayerId, Player>({{0, {"Player 1", "[::1]:10"}}, {1, {"Player 2", "[::1]:11"}}})));
            l.count_down_and_wait();
            l.reset(2);
            while (true) {
                puts("CZEKAM");
                ClientMessage min;
                *socks[n] >> min;
                puts("MAM");
                visit([this, n](auto &&v) {
                    using dupa = decay_t<typeof(v)>;
                    if constexpr (is_same_v<dupa, MoveMessage>) {
                        puts("GOWNO");
                        if (v.getDirection() == Direction::Up) {
                            posa[n] = Position(posb[n], Direction::Up, 1);
                        } else if (v.getDirection() == Direction::Down) {
                            posa[n] = Position(posb[n], Direction::Down, 1);
                        }
                    }
                }, min);
            }
        };
        boost::thread t1(work, 0);
        boost::thread t2(work, 1);
        boost::thread srv([this]() {
            bool first = true;
            l.wait();
            while (true) {
                std::vector<Event> events;
                for (uint8_t n = 0; n <= 1; n++) {
                    posb[n] = posa[n];
                    events.emplace_back(PlayerMovedEvent(n, posb[n]));
                }
                shared_ptr<BombExplodedEvent> ev;
                if (pball.x == 9)
                    vhball = Direction::Left;
                if (pball.x == 0)
                    vhball = Direction::Right;
                if (pball.y == 5)
                    vvball = Direction::Down;
                if (pball.y == 0)
                    vvball = Direction::Up;
                if (pball.x == 0 && posb[0].y != pball.y)
                    ev = make_shared<BombExplodedEvent>(
                            BombExplodedEvent(0, std::vector<PlayerId>({0}), std::vector<Position>()));
                else if (pball.x == 9 && posb[1].y != pball.y)
                    ev = make_shared<BombExplodedEvent>(
                            BombExplodedEvent(0, std::vector<PlayerId>({1}), std::vector<Position>()));
                else
                    ev = make_shared<BombExplodedEvent>(
                            BombExplodedEvent(0, std::vector<PlayerId>(), std::vector<Position>()));
                if (!first) {
                    events.push_back(*ev);
                }
                Position next(pball, vhball, 1);
                pball = Position(next, vvball, 1);
                events.emplace_back(BombPlacedEvent(0, pball));
                for (uint8_t n = 0; n <= 1; n++) {
                    *socks[n] << ServerMessage(TurnMessage(0, events));
                }
                this_thread::sleep_for(std::chrono::milliseconds(turn_duration));
                first = false;
            }
        });
        srv.join();
    }
};

int main(int argc, char **argv) {
    if (argc != 2)
        throw std::invalid_argument(string().append("Usage: ").append(argv[0]).append(" <port>"));
    Work(uint16_t(stoi(argv[1])));
}
