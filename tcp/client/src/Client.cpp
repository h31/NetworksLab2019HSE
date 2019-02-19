#include <iostream>
#include <Client.h>
#include <unordered_map>
#include <unistd.h>

#include "Client.h"

Client::Client(int socketfd) : socketfd(socketfd) {
    commands["new rating"] = [this] { return newRating(); };
    commands["delete rating"] = [this] { return deleteRating(); };
    commands["open rating"] = [this] { return openRating(); };
    commands["close rating"] = [this] { return closeRating(); };
    commands["list ratings"] = [this] { return listRatings(); };
    commands["add option"] = [this] { return addOption(); };
    commands["show rating"] = [this] { return showRating(); };
    commands["vote"] = [this] { return vote(); };
    commands["help"] = [this] { help(); return true; };
    commands["exit"] = [] { return false; };
}

void Client::run() {
    if (!connect()) {
        return;
    }
    while (true) {
        std::cout << "> ";
        std::cout.flush();
        std::string command;
        if (!getline(std::cin, command)) {
            break;
        }
        if (command.empty()) {
            std::cout << std::endl;
            continue;
        }
        if (!commands.count(command)) {
            std::cerr << "Invalid command" << std::endl;
            help();
            continue;
        }
        if (!commands[command]()) {
            break;
        }
    }
}

bool Client::connect() {
    while (true) {
        std::cout << "Enter your username:" << std::endl;
        std::string username;
        if (!getline(std::cin, username)) {
            return false;
        }
        Response response = performRequest(
                RequestType::CONNECT, { RequestField(username) });
        if (response.checkDisconnect()) {
            return false;
        }
        if (!response.isError()) {
            return true;
        }
        response.print();
    }
}

void Client::help() {
    std::cout << "Valid commands:" << std::endl;
    for (auto &command : commands) {
        std::cout << command.first << std::endl;
    }
}

bool Client::newRating() {
    std::cout << "Enter rating name:" << std::endl;
    std::string name;
    if (!getline(std::cin, name)) {
        return false;
    }
    int options = 0;
    while (true) {
        std::cout << "Enter number of options:" << std::endl;
        if (!(std::cin >> options)) {
            return false;
        }
        if (0 < options && options < 256) {
            break;
        }
        std::cerr << "Invalid number of options. Must be in range [1..255]" << std::endl;
    }
    Response response = performRequest(
            RequestType::NEW_RATING, { RequestField(name),RequestField(static_cast<uint8_t>(options)) }
    );
    if (response.checkDisconnect()) {
        return false;
    }
    response.print();
    return true;
}

bool Client::deleteRating() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return false;
    }
    Response response = performRequest( RequestType::DELETE_RATING, { RequestField(id) });
    if (response.checkDisconnect()) {
        return false;
    }
    response.print();
    return true;
}

bool Client::openRating() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return false;
    }
    Response response = performRequest(RequestType::OPEN_RATING, { RequestField(id) });
    if (response.checkDisconnect()) {
        return false;
    }
    response.print();
    return true;
}

bool Client::closeRating() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return false;
    }
    Response response = performRequest(RequestType::CLOSE_RATING, { RequestField(id) });
    if (response.checkDisconnect()) {
        return false;
    }
    if (response.isError()) {
        std::cerr << "Error: " << response.getError() << std::endl;
    }
    return true;
}

bool Client::addOption() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return false;
    }
    std::cout << "Enter option title:" << std::endl;
    std::string option;
    getline(std::cin, option); // just to skip newline
    if (!getline(std::cin, option)) {
        return false;
    }
    Response response = performRequest(
            RequestType::ADD_OPTION, { RequestField(id), RequestField(option) }
    );
    if (response.checkDisconnect()) {
        return false;
    }
    response.print();
    return true;
}

bool Client::listRatings() {
    Response response = performRequest(RequestType::LIST_RATINGS, { });
    if (response.checkDisconnect()) {
        return false;
    }
    response.print();
    return true;

}

bool Client::showRating() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return false;
    }
    Response response = performRequest(RequestType::GET_RATING, { RequestField(id) });
    if (response.checkDisconnect()) {
        return false;
    }
    response.print();
    return true;
}

bool Client::vote() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return false;
    }
    int option = 0;
    while (true) {
        std::cout << "Enter option index:" << std::endl;
        if (!(std::cin >> option)) {
            return false;
        }
        if (0 < option && option < 256) {
            break;
        }
        std::cerr << "Invalid option index. Must be in range [1..255]" << std::endl;
    }
    Response response = performRequest(
            RequestType::VOTE, { RequestField(id), RequestField(static_cast<uint8_t>(option))}
    );
    if (response.checkDisconnect()) {
        return false;
    }
    response.print();
    return true;
}

Response Client::performRequest(Client::RequestType type, std::vector<RequestField> fields) {
    if (write(socketfd, &type, sizeof(type)) != sizeof(type)) {
        return Response::ResponseDisconnect();
    }
    for (auto &field : fields) {
        field.write(socketfd);
    }
    return Response::readResponse(socketfd);
}
