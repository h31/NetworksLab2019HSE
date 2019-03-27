#include <netdb.h>
#include <thread>
#include "../include/client.h"

bool MarketClient::StartClient(const char* host, uint16_t port_number) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd_ < 0) {
        std::cerr << "failed to open socket\n";
        exit(1);
    }

    hostent* server = gethostbyname(host);

    if (server == nullptr) {
        std::cerr << "host not found\n";
        exit(0);
    }

    sockaddr_in serv_addr{0};
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, static_cast<void*>(&serv_addr.sin_addr.s_addr),
          static_cast<size_t>(server->h_length));
    serv_addr.sin_port = htons(port_number);

    if (connect(sockfd_, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        std::cerr << "connection failed\n";
        exit(1);
    }

    PrintHeader("AUTHORISATION");

    return true;
}

bool MarketClient::AuthoriseCustomer(const std::string& name) {
    Message response = SendMessage(Message::NEW_CUSTOMER, name);
    switch (response.type) {
        case Message::CUSTOMER_ADDED:
            std::cout << "Authorisation successful, you can create orders now, " + name + "\n";
            return true;
        case Message::CANT_ADD_CUSTOMER:
            std::cout << response.body.empty() ? "Sorry, something went wrong" : response.body +
                                                                                 "\n";
            return false;
        default:
            HandleResponse(response);
            return false;
    }

}

bool MarketClient::AuthoriseFreelancer(const std::string& name) {
    Message response = SendMessage(Message::NEW_FREELANCER, name);
    switch (response.type) {
        case Message::FREELANCER_ADDED:
            std::cout << "You can look for orders now\n";
            return true;
        case Message::CANT_ADD_FREELANCER:
            std::cout << response.body.empty() ? "Sorry, something went wrong\n" : response.body +
                                                                                   "\n";
            return false;
        default:
            HandleResponse(response);
            return false;
    }
}

void MarketClient::ListMyOrders() {
    Message response = SendMessage(Message::GET_MY_ORDERS);
    switch (response.type) {
        case Message::LIST_OF_MY_ORDERS:
            std::cout << response.body << "\n";
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void MarketClient::ListOpenOrders() {
    Message response = SendMessage(Message::GET_OPEN_ORDERS);
    switch (response.type) {
        case Message::LIST_OF_OPEN_ORDERS:
            std::cout << response.body << "\n";
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void MarketClient::HandleUnexpectedServerResponse(const Message& response) {
    std::cout << "server responded with {type: " + std::to_string(response.type) + ", message: " +
                 "}\n";
}

void MarketClient::HandleUnauthorised() { std::cout << "Please enter who u are\n"; }

void MarketClient::HandleIncorrectMessage(const Message& response) {
    std::cout << response.body.empty() ? "You are not permitted to do it" : response.body + "\n";
}

void MarketClient::HandleResponse(const Message& response) {
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

void MarketClient::RequestOrder(int order_id) {
    Message response = SendMessage(Message::TAKE_ORDER, std::to_string(order_id));
    switch (response.type) {
        case Message::TAKE_ORDER_SUCCESSFUL:
            std::cout << "Order" + response.body + " requested.\n";
            return;
        case Message::TAKE_ORDER_NOT_SUCCESSFUL:
            std::cout << response.body << "\n";
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void MarketClient::StartOrder(int order_id) {
    Message response = SendMessage(Message::WORK_STARTED, std::to_string(order_id));
    switch (response.type) {
        case Message::WORK_STARTED_SUCCESSFUL:
            std::cout << "Order" + response.body + " started successfully.\n";
            return;
        case Message::WORK_STARTED_NOT_SUCCESSFUL:
            std::cout << response.body << "\n";
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void MarketClient::FinishOrder(int order_id) {
    Message response = SendMessage(Message::WORK_FINISHED, std::to_string(order_id));
    switch (response.type) {
        case Message::WORK_FINISHED_SUCCESSFUL:
            std::cout << "Order" + response.body + " finished successfully.\n";
            return;
        case Message::WORK_FINISHED_NOT_SUCCESSFUL:
            std::cout << response.body << "\n";
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void MarketClient::NewOrder(const std::string& description) {
    Message response = SendMessage(Message::NEW_ORDER, description);
    switch (response.type) {
        case Message::ORDER_ACCEPTED:
            std::cout << "You can now track this order by id: " + response.body + "\n";
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void MarketClient::GiveOrder(int order_id, const std::string& name) {
    Message response = SendMessage(Message::GIVE_ORDER_TO_FREELANCER,
                                   std::to_string(order_id) + name);
    switch (response.type) {
        case Message::GIVE_ORDER_SUCCESSFUL:
            std::cout << "Order" + response.body + " given successfully.\n";
            return;
        case Message::GIVE_ORDER_NOT_SUCCESSFUL:
            std::cout << response.body << "\n";
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void MarketClient::ApproveDoneOrder(int order_id) {
    Message response = SendMessage(Message::WORK_ACCEPTED, std::to_string(order_id));
    switch (response.type) {
        case Message::WORK_ACCEPTED_SUCCESSFUL:
            std::cout << "Work for order" + response.body + " accepted successfully.\n";
            return;
        case Message::WORK_ACCEPTED_NOT_SUCCESSFUL:
            std::cout << response.body << "\n";
            return;
        default:
            HandleResponse(response);
            return;
    }
}

void MarketClient::Quit() {
    std::cout << "Closing connection...\n";
    shutdown(sockfd_, SHUT_RDWR);
    std::cout << "Bye! Hope to see u again!";
}

void MarketClient::PrintHeader(const std::string& header) {
    system("clear");
    std::cout << header + "\n> " << std::flush;
}

void MarketClient::PrintPrompt() {
    std::cout << "> " << std::flush;
}

bool MarketClient::GetLine(std::string& message) {
    auto& result = getline(std::cin, message);
    return bool(result);
}

Message MarketClient::SendMessage(Message::Type type) {
    return SendMessage(type, "");
}

Message MarketClient::SendMessage(Message::Type type, const std::string& text) {
    Message request{type, text};
    request.Write(sockfd_);
    return Message::Read(sockfd_);
}
