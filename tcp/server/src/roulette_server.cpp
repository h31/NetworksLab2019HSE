#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <thread>
#include <boost/lockfree/queue.hpp>
#include "../include/roulette_server.h"


void RouletteServer::StartWorkingWithClient(int sock_fd) {
    ClientStatus status = ClientStatus::NEW;
    Player* player;
    while (status != ClientStatus::FINISH) {
        Message message = Message::Read(sock_fd);
        ClientStatus request_status;
        if (status == ClientStatus::NEW) {
            request_status = WorkWithUnauthorized(sock_fd, message, &player);
        } else if (status == ClientStatus::PLAYER) {
            request_status = WorkWithPlayer(player, message);
        } else {
            request_status = WorkWithCroupier(sock_fd, message);
        }

        if (request_status != UNCHANGED) {
            status = request_status;
        }
    }
}

RouletteServer::ClientStatus RouletteServer::WorkWithUnauthorized(
        int sock_fd, Message message, Player** player) {
    Message::Type ans_type;
    ClientStatus status = ClientStatus::NEW;
    if (Message::NEW_PLAYER == message.type) {
        std::lock_guard<std::mutex> players_lock(players_mutex_);
        if (players.count(message.body) == 0) {
            *player = new Player(message.body, sock_fd);
            players[message.body] = *player;
            ans_type = Message::PLAYER_ADDED;
            std::cout << "New player\n";
            status = ClientStatus::PLAYER;
        } else {
            ans_type = Message::CANT_ADD_PLAYER;
        }
    } else if (Message::NEW_CROUPIER == message.type) {
        std::lock_guard<std::mutex> croupier_lock(croupier_mutex_);
        if (croupier_socket_) {
            ans_type = Message::CROUPIER_ALREADY_EXISTS;
        } else if (message.body == CROUPIER_PASSWORD) {
            croupier_socket_ = sock_fd;
            ans_type = Message::CROUPIER_ADDED;
            std::cout << "New croupier\n";
            status = ClientStatus::CROUPIER;
        } else {
            ans_type = Message::CANT_ADD_CROUPIER;
        }
    } else if (Message::UNDEFINED == message.type) {
        close(sock_fd);
        return ClientStatus::FINISH;
    } else {
        ans_type = Message::UNAUTHORIZED;
    }

    Message ans_message(ans_type);
    ans_message.Write(sock_fd);
    return status;
}


RouletteServer::ClientStatus RouletteServer::WorkWithCroupier(int sock_fd, const Message& message) {
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
            croupier_socket_ = 0;
            croupier_mutex_.unlock();
            close(sock_fd);
            std::cout << "Croupier left\n";
            return ClientStatus::FINISH;
        }

        default: {
            ans_message = Message(Message::INCORRECT_MESSAGE);
        }

    }
    ans_message.Write(sock_fd);
    return ClientStatus::UNCHANGED;
}

RouletteServer::ClientStatus
RouletteServer::WorkWithPlayer(Player* player, const Message& message) {
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
            return ClientStatus::FINISH;
        }

        default: {
            ans_message = Message(Message::INCORRECT_MESSAGE);
        }
    }
    player->messages_.push(ans_message);
    return ClientStatus::UNCHANGED;

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
        std::lock_guard<std::mutex> players_lock(players_mutex_);
        for (auto& player: players) {
            int winning_number = rand() % AMOUNT_OF_NUMBERS;
            int profit = player.second->CalculateProfit(winning_number);
            Message msg(Message::DRAW_RESULTS,
                        std::to_string(winning_number) + " " + std::to_string(profit));
            player.second->bet = 0;
            player.second->bet_type = Player::NO_BET;
            player.second->messages_.push(msg);
        }
        is_rolling_ = false;
        rolling_mutex_.unlock();
        return Message(Message::DRAW_ENDED);
    }
    return Message(Message::CANT_END_DRAW, "Game wasnt in progress.");
}

Message RouletteServer::ProcessGetAllBets() {
    std::lock_guard<std::mutex> players_lock(players_mutex_);
    std::string result;
    for (auto& p: players) {
        result += p.first;
        result += " ";
        result += std::to_string(p.second->bet);
        result += '\n';
    }
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

    std::lock_guard<std::mutex> players_lock(players_mutex_);
    Message::Type result;
    if (player.bet != 0) {
        result = Message::REPEATED_BET;
    } else {
        player.bet = bet_sum;
        player.bet_type = bet_type;
        player.number = betting_number;
        result = Message::BET_ACCEPTED;
    }
    return Message(result);
}

RouletteServer::~RouletteServer() {
    for (const auto& p: players) {
        shutdown(p.second->socket_fd, SHUT_RDWR);
    }
    if (croupier_socket_) {
        shutdown(croupier_socket_, SHUT_RDWR);
    }
    for (auto& t: threads_) {
        t.join();
    }
}

RouletteServer::RouletteServer() {
    srand(time(NULL));
}

void RouletteServer::DeletePlayer(RouletteServer::Player* player) {
    std::lock_guard<std::mutex> players_lock(players_mutex_);
    players.erase(player->name);
    delete player;
}

bool RouletteServer::BanPlayer(const std::string& name) {
    bool result = false;
    std::lock_guard<std::mutex> players_lock(players_mutex_);
    if (players.count(name)) {
        close(players[name]->socket_fd);
        result = true;
    }
    return result;
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
