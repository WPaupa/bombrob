#ifndef BOMBOWE_ROBOTY_CLIENT_H
#define BOMBOWE_ROBOTY_CLIENT_H

#include "client_options.h"
#include "../common/sock_stream.h"
#include "../common/types.h"
#include "../common/message.h"
#include <map>
#include <set>
#include <vector>

class Client {
private:
    TCPClientSockStream server;
    UDPSockStream display;
    // Zmienna lobby jest jedyną zmienną klienta, z której korzystają
    // obydwa wątki. Nie musimy jednak czynić jej atomową, bo każdy
    // przeplot wątków jest w pełni szeregowalny ze względu na tę zmienną.
    bool lobby;
    std::string server_name;
    uint16_t size_x;
    uint16_t size_y;
    uint16_t game_length;
    uint16_t explosion_radius;
    uint16_t bomb_timer;
    std::map<PlayerId, Player> players;
    std::map<PlayerId, Position> player_positions;
    std::vector<Position> blocks;
    std::map<BombId, Bomb> bomb_ids;
    std::vector<Bomb> bombs;
    std::vector<Position> explosions;
    std::map<PlayerId, Score> scores;
    uint8_t players_count;
    uint16_t turn;
    std::string player_name;

    // Metoda parseFromServer jest przeciążona dla każdego typu wiadomości od serwera,
    // żeby można było wywołać ją w std::visit
    void parseFromServer(HelloMessage &message);
    void parseFromServer(AcceptedPlayerMessage &message);
    void parseFromServer(GameStartedMessage &message);
    void parseFromServer(TurnMessage &message);
    void parseFromServer(GameEndedMessage &message);

    // Metoda parseFromDisplay jest taka sama dla każdego typu wiadomości od GUI,
    // więc łatwiej ją przeciążyć szablonem. Jest prywatna, więc wrzucanie
    // do niej konceptu byłoby śmiesznym, acz niepraktycznym żartem.
    template<typename T>
    void parseFromDisplay(T &message);

    void sendToDisplay();

public:
    // Konstruktor klasy Client tworzy nowego klienta, a więc łączy się z serwerem
    // i wykonuje całą logikę. Można z niego wyjść tylko przez rzucenie wyjątku
    explicit Client(ClientOptions &options);
};


#endif //BOMBOWE_ROBOTY_CLIENT_H
