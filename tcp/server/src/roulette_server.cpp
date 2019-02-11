#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <thread>

#include "roulette_server.h"


void RouletteServer::StartWorkingWithClient(int sock_fd) {
    std::function<void()> workWithClient = nullptr;
    while (true) {
        Message message = Message::Read(sock_fd);
        Message::Type ans_type;
        if (Message::NEW_PLAYER == message.type) {
            players_mutex_.lock();
            if (players.count(message.body) == 0) {
                auto* player = new Player(message.body, sock_fd);
                players[message.body] = player;
                workWithClient = std::bind(&RouletteServer::WorkWithPlayer, this, player);
                ans_type = Message::PLAYER_ADDED;
                std::cout << "New player\n";
            } else {
                ans_type = Message::CANT_ADD_PLAYER;
            }
            players_mutex_.unlock();
        } else if (Message::NEW_CROUPIER == message.type) {
            croupier_mutex_.lock();
            if (have_croupier_) {
                ans_type = Message::CROUPIER_ALREADY_EXISTS;
            } else if (message.body == CROUPIER_PASSWORD) {
                have_croupier_ = true;
                workWithClient = std::bind(&RouletteServer::WorkWithCroupier, this, sock_fd);
                ans_type = Message::CROUPIER_ADDED;
                std::cout << "New croupier\n";
            } else {
                ans_type = Message::CANT_ADD_CROUPIER;
            }
            croupier_mutex_.unlock();
        } else {
            ans_type = Message::UNAUTHORIZED;
        }

        Message ans_message(ans_type);
        ans_message.Write(sock_fd);

        if (workWithClient != nullptr) {
            break;
        }
    }
    workWithClient();
}

void RouletteServer::WorkWithCroupier(int sock_fd) {
    while (true) {
        Message message = Message::Read(sock_fd);
        Message ans_message;
        switch (message.type) {
            case Message::START_DRAW: {
                ans_message = ProcessStartDraw();
                break;
            }
            case Message::END_DRAW: {
                ans_message = ProcessEndDraw();
                break;
            }
            case Message::GET_ALL_BETS: {
                ans_message = ProcessGetAllBets();
                break;
            }
            case Message::UNDEFINED: {
                croupier_mutex_.lock();
                have_croupier_ = false;
                croupier_mutex_.unlock();
                close(sock_fd);
                std::cout << "Croupier left\n";
                return;
            }

            default: {
                ans_message = Message(Message::INCORRECT_MESSAGE);
            }

        }
        ans_message.Write(sock_fd);
    }
}

void RouletteServer::WorkWithPlayer(Player* player) {
    while (true) {
        Message message = Message::Read(player->socket_fd);
        Message ans_message;
        switch (message.type) {
            case Message::NEW_BET: {
                if (!rolling_mutex_.try_lock_shared()) {
                    ans_message = Message(Message::UNTIMELY_BET);
                    break;
                }

                // Otherwise game wasn't started.
                ans_message = ProcessBet(*player, message.body);
                rolling_mutex_.unlock_shared();
                break;
            }
            case Message::GET_ALL_BETS: {
                ans_message = ProcessGetAllBets();
                break;
            }

            case Message::UNDEFINED: {
                DeletePlayer(player);
                std::cout << "Player left\n";
                return;
            }

            default: {
                ans_message = Message(Message::INCORRECT_MESSAGE);
            }
        }
        player->messages_.push(ans_message);
    }
}

Message RouletteServer::ProcessStartDraw() {
    if (!is_rolling_) {
        rolling_mutex_.lock();
        is_rolling_ = true;
        std::cout << "New spin\n";
        return Message(Message::DRAW_STARTED);
    }
    return Message(Message::CANT_START_DRAW, "Game is already in progress.");

}

Message RouletteServer::ProcessEndDraw() {
    if (is_rolling_) {
        players_mutex_.lock();
        for (auto& player: players) {
            int winning_number = rand() % AMOUNT_OF_NUMBERS;
            int profit = player.second->CalculateProfit(winning_number);
            Message msg(Message::DRAW_RESULTS,
                        std::to_string(winning_number) + " " + std::to_string(profit));
            player.second->bet = 0;
            player.second->bet_type = Player::NO_BET;
            player.second->messages_.push(msg);
        }
        players_mutex_.unlock();
        rolling_mutex_.unlock();
        is_rolling_ = false;
        return Message(Message::DRAW_ENDED);
    }
    return Message(Message::CANT_END_DRAW, "Game wasnt in progress.");
}

Message RouletteServer::ProcessGetAllBets() {
    players_mutex_.lock();
    std::string result;
    for (auto& p: players) {
        result += p.first;
        result += " ";
        result += std::to_string(p.second->bet);
        result += '\n';
    }
    players_mutex_.unlock();
    return Message(Message::LIST_OF_BETS, result);
}

Message RouletteServer::ProcessBet(RouletteServer::Player& player, std::string bet) {
    std::string bet_type_str;
    Player::BetType bet_type;
    int betting_number = -1;
    size_t delimeter_pos = bet.find(' ');
    bet_type_str = bet.substr(0, delimeter_pos);
    if (bet_type_str == "even") {
        bet_type = Player::EVEN;
    } else if (bet_type_str == "odd") {
        bet_type = Player::ODD;
    } else {
        try {
            betting_number = std::stoi(bet_type_str);
        } catch (...) {
            return Message(Message::INCORRECT_MESSAGE, "Incorrect bet type " + bet_type_str);
        }
        if (betting_number >= 0 && betting_number < AMOUNT_OF_NUMBERS) {
            bet_type = Player::NUMBER;
        } else {
            return Message(Message::INCORRECT_MESSAGE, "Incorrect bet type " + bet_type_str);
        }
    }

    int bet_sum = 0;
    try {
        bet_sum = std::stoi(bet.substr(delimeter_pos, bet.size() - delimeter_pos));
    } catch (...) {
        return Message(Message::INCORRECT_MESSAGE,
                       "Incorrect bet sum " +
                       bet.substr(delimeter_pos, bet.size() - delimeter_pos));
    }

    players_mutex_.lock();
    Message::Type result;
    if (player.bet != 0) {
        result = Message::REPEATED_BET;
    } else {
        player.bet = bet_sum;
        player.bet_type = bet_type;
        player.number = betting_number;
        result = Message::BET_ACCEPTED;
    }
    players_mutex_.unlock();
    return Message(result);
}

RouletteServer::~RouletteServer() {
    for (const auto& p: players) {
        close(p.second->socket_fd);
    }
}

RouletteServer::RouletteServer() {
    srand(time(NULL));
}

void RouletteServer::DeletePlayer(RouletteServer::Player* player) {
    players_mutex_.lock();
    players.erase(player->name);
    players_mutex_.unlock();

    delete player;
}

int RouletteServer::Player::CalculateProfit(int winning_number) {
    if ((winning_number % 2 == 0 && bet_type == EVEN && winning_number != 0) ||
        (winning_number % 2 == 1 && bet_type == ODD))
        return 2 * bet;
    if (winning_number == number && bet_type == NUMBER)
        return 35 * bet;
    return 0;
}

RouletteServer::Player::Player(const std::string& name, int socket_fd) : name(name),
                                                                         socket_fd(socket_fd) {
    writer = std::thread([this]() {
        while (!messages_.closed()) {
            try {
                Message m = messages_.pull();
                m.Write(this->socket_fd);
            } catch (boost::sync_queue_is_closed&) {
                break;
            }
        }
    });
}

RouletteServer::Player::~Player() {
    messages_.close();
    close(socket_fd);
    writer.join();
}
