#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <map>
#include "../../message/src/message.h"

class RouletteServer {
 public:
    void StartServer(uint16_t port_number);

    ~RouletteServer();
 private:
    struct Player {
        explicit Player(int socket_fd) : socket_fd(socket_fd) {};
        const int socket_fd;

        int bet = 0;
        enum BetType {
            NO_BET,
            EVEN,
            ODD,
            NUMBER
        };
        BetType bet_type = NO_BET;
        int number = -1;

        int CalculateProfit(int winning_number);
    };

    const char* CROUPIER_PASSWORD = "ANDREY_THE_BEST";
    const int AMOUNT_OF_NUMBERS = 37;
    bool have_croupier_ = false;
    bool is_rolling_ = false;

    std::map<std::string, Player*> players;

    std::mutex players_mutex_;
    std::mutex croupier_mutex_;

    std::shared_timed_mutex rolling_mutex_;

    Message ProcessStartDraw();

    Message ProcessEndDraw();

    void AuthoriseClient(int sock_fd);

    void WorkWithCroupier(int sock_fd);

    void WorkWithPlayer(Player* player);

    Message ProcessGetAllBets();

    Message ProcessBet(Player& player, std::string bet);

};

#endif //SERVER_SERVER_H
