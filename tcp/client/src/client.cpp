#include <netdb.h>
#include <boost/lockfree/queue.hpp>
#include <thread>
#include "client.h"

bool RouletteClient::StartClient(const char *host, uint16_t port_number) {

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd_ < 0) {
        std::cerr << "failed to open socket\n";
        exit(1);
    }

    hostent *server = gethostbyname(host);

    if (server == nullptr) {
        std::cerr << "host not found\n";
        exit(0);
    }

    sockaddr_in serv_addr{0};
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, static_cast<void *>(&serv_addr.sin_addr.s_addr), static_cast<size_t>(server->h_length));
    serv_addr.sin_port = htons(port_number);

    if (connect(sockfd_, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
        std::cerr << "connection failed\n";
        exit(1);
    }

    PrintHeader("AUTHORISATION");

    request_sender_ = std::thread([this]() {
        while (!requests_.closed()) {
            try {
                Message m = requests_.pull();
                m.Write(sockfd_);
            } catch (boost::sync_queue_is_closed &) {
                break;
            }
        }
    });

    response_receiver_ = std::thread([this]() {
        while (!responses_.closed()) {
            try {
                Message m = Message::Read(sockfd_);
                if (m.type == Message::DRAW_RESULTS) {
                    int number, win;
                    std::sscanf(m.body.c_str(), "%d %d", &number, &win);
                    PrintHeader("DRAW ENDED\nWINNING NUMBER " + std::to_string(number) +
                                "\nYOUR PROFIT " + std::to_string(win));
                } else {
                    responses_.push(m);
                }
            } catch (boost::sync_queue_is_closed &) {
                break;
            }
        }
    });

    return true;
}

void RouletteClient::AuthorisePlayer(const std::string &name) {
    Message request{Message::NEW_PLAYER, name};
    requests_.push(request);
    Message response = responses_.pull();
    switch (response.type) {
        case Message::PLAYER_ADDED:
            Cout("enjoy the game, " + name);
            return;
        case Message::CANT_ADD_PLAYER:
            Cout(response.body.empty() ? "sorry, this name is taken" : response.body);
            return;
        default:
            HandleResponse(response);
            return;
    }

}

void RouletteClient::AuthoriseCroupier(const std::string &key) {
    Message request{Message::NEW_CROUPIER, key};
    requests_.push(request);
    Message response = responses_.pull();
    switch (response.type) {
        case Message::CROUPIER_ADDED:
            Cout("welcome ruler");
            return;
        case Message::CANT_ADD_CROUPIER:
            Cout(response.body.empty() ? "wrong password" : response.body);
            return;
        case Message::CROUPIER_ALREADY_EXISTS:
            Cout(response.body.empty() ? "there can't be two masters" : response.body);
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void RouletteClient::StartDraw() {
    Message request{Message::START_DRAW};
    requests_.push(request);
    Message response = responses_.pull();
    switch (response.type) {
        case Message::DRAW_STARTED:
            Cout("whirrr!");
            return;
        case Message::CANT_START_DRAW:
            Cout(response.body.empty() ? "draw declined" : response.body);
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void RouletteClient::FinishDraw() {
    Message request{Message::END_DRAW};
    requests_.push(request);
    Message response = responses_.pull();
    switch (response.type) {
        case Message::DRAW_ENDED:
            Cout("draw ended");
            return;
        case Message::CANT_END_DRAW:
            Cout(response.body.empty() ? "can't finish" : response.body);
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void RouletteClient::NewBet(const std::string &type, int sum) {
    Message request{Message::NEW_BET, type + " " + std::to_string(sum)};
    requests_.push(request);
    Message response = responses_.pull();
    switch (response.type) {
        case Message::BET_ACCEPTED:
            Cout("good luck!");
            return;
        case Message::REPEATED_BET:
            Cout(response.body.empty() ? "you already made a bet" : response.body);
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void RouletteClient::ListBets() {
    Message request{Message::GET_ALL_BETS};
    requests_.push(request);
    Message response = responses_.pull();
    switch (response.type) {
        case Message::LIST_OF_BETS:
            Cout(response.body);
            return;
        default:
            HandleResponse(response);
            return;

    }
}

void RouletteClient::HandleUnexpectedServerResponse(const Message &response) {
    Cout("server responded with {type: " + std::to_string(response.type) + ", message: " + response.body + "}");
}

void RouletteClient::HandleUnauthorised() { Cout("introduce yourself"); }

void RouletteClient::HandleIncorrectMessage(const Message &response) {
    Cout(response.body.empty() ? "unexpected request" : response.body);
}

void RouletteClient::HandleResponse(const Message &response) {
    switch (response.type) {
        case Message::INCORRECT_MESSAGE:
            HandleIncorrectMessage(response);
            return;
        case Message::UNAUTHORIZED:
            HandleUnauthorised();
            return;
        case Message::UNDEFINED:
            Quit();
            exit(0);
        default:
            HandleUnexpectedServerResponse(response);
            return;
    }
}

void RouletteClient::Quit() {
    Cout("closing connection...");
    shutdown(sockfd_, SHUT_RDWR);
    requests_.close();
    responses_.close();
    request_sender_.join();
    response_receiver_.join();
    Cout("bye!");
}

void RouletteClient::PrintHeader(const std::string &header) {
    io_mutex_.lock();
    system("clear");
    std::cout << header + "\n> " << std::flush;
    io_mutex_.unlock();
}

void RouletteClient::PrintPrompt() {
    io_mutex_.lock();
    std::cout << "> " << std::flush;
    io_mutex_.unlock();
}

void RouletteClient::Cout(const std::string &message) {
    io_mutex_.lock();
    std::cout << message << std::endl;
    io_mutex_.unlock();
}

bool RouletteClient::GetLine(std::string &message) {
    auto &result = getline(std::cin, message);
    return bool(result);
}

