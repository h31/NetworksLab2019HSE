#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <map>
#include <boost/thread/sync_queue.hpp>
#include "../include/message.h"
#include "tcp_server.h"

class RouletteServer : public TcpServer {
 public:
    RouletteServer();

    ~RouletteServer();

    bool BanPlayer(const std::string& name);

 private:
    class Player {
     public:
        explicit Player(const std::string& name, int socket_fd);

        ~Player();

        const int socket_fd;

        const std::string name;

        int bet = 0;
        enum BetType {
            NO_BET,
            EVEN,
            ODD,
            NUMBER
        };
        BetType bet_type = NO_BET;
        int number = -1;

        std::thread writer;
        boost::sync_queue<Message> messages_;

        int CalculateProfit(int winning_number);
    };

    enum ClientStatus {
        NEW,
        PLAYER,
        CROUPIER,
        FINISH,
        UNCHANGED
    };

    const char* CROUPIER_PASSWORD = "ANDREY_THE_BEST";
    const int AMOUNT_OF_NUMBERS = 37;
    int croupier_socket_ = 0;
    bool is_rolling_ = false;

    std::map<std::string, Player*> players;

    std::mutex players_mutex_;
    std::mutex croupier_mutex_;

    std::shared_timed_mutex rolling_mutex_;

    Message ProcessStartDraw();

    Message ProcessEndDraw();

    void StartWorkingWithClient(int sock_fd) override;

    ClientStatus WorkWithCroupier(int sock_fd, const Message& message);

    ClientStatus WorkWithPlayer(Player* player, const Message& message);

    ClientStatus WorkWithUnauthorized(int sock_fd, Message message, Player** player);

    Message ProcessGetAllBets();

    Message ProcessBet(Player& player, std::string bet);

    void DeletePlayer(Player* player);
};

#endif //SERVER_SERVER_H
