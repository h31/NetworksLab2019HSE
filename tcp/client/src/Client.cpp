#include <iostream>
#include <Client.h>
#include <unordered_map>
#include <unistd.h>

#include "Client.h"

Client::Client(int socketfd) : socketfd(socketfd), reader(SocketReader(socketfd)) {
    commands["new rating"] = [this] { return newRating(); };
    commands["delete rating"] = [this] { return deleteRating(); };
    commands["open rating"] = [this] { return openRating(); };
    commands["close rating"] = [this] { return closeRating(); };
    commands["list ratings"] = [this] { return listRatings(); };
    commands["add option"] = [this] { return addOption(); };
    commands["show rating"] = [this] { return showRating(); };
    commands["vote"] = [this] { return vote(); };
    commands["exit"] = [] { return Response::exitResponse(); };
}

void Client::run() {
    if (authorize().checkExit()) {
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
        if (command == "exit") {
            break;
        }
        if (!commands.count(command)) {
            if (command != "help") {
                std::cerr << "Invalid command" << std::endl;
            }
            help();
            continue;
        }
        Response response = commands[command]();
        response.print();
        if (response.checkExit()) {
            return;
        }
    }
}

Response Client::authorize() {
    while (true) {
        std::cout << "Enter your username:" << std::endl;
        std::string username;
        if (!getline(std::cin, username)) {
            return Response::exitResponse();
        }
        Response response = performRequest(RequestType::AUTHORIZE, { RequestField(username) });
        if (response.isError()) {
            response.print();
        } else {
            return response;
        }
    }
}

void Client::help() {
    std::cout << "Valid commands:" << std::endl;
    for (auto &command : commands) {
        std::cout << command.first << std::endl;
    }
}

Response Client::newRating() {
    std::cout << "Enter rating name:" << std::endl;
    std::string name;
    if (!getline(std::cin, name)) {
        return Response::exitResponse();
    }
    int options = 0;
    while (true) {
        std::cout << "Enter number of options:" << std::endl;
        if (!(std::cin >> options)) {
            return Response::exitResponse();
        }
        if (options < 1 || 255 < options) {
            std::cerr << "Invalid number of options. Must be in range [1..255]" << std::endl;
        } else {
            break;
        }
    }
    return performRequest(RequestType::NEW_RATING, { RequestField(name),RequestField(static_cast<uint8_t>(options)) });
}

Response Client::deleteRating() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return Response::exitResponse();
    }
    return performRequest( RequestType::DELETE_RATING, { RequestField(id) });
}

Response Client::openRating() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return Response::exitResponse();
    }
    return performRequest(RequestType::OPEN_RATING, { RequestField(id) });
}

Response Client::closeRating() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return Response::exitResponse();
    }
    return performRequest(RequestType::CLOSE_RATING, { RequestField(id) });
}

Response Client::addOption() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return Response::exitResponse();
    }
    std::cout << "Enter option title:" << std::endl;
    std::string option;
    getline(std::cin, option); // just to skip newline
    if (!getline(std::cin, option)) {
        return Response::exitResponse();
    }
    return performRequest(RequestType::ADD_OPTION, { RequestField(id), RequestField(option) });
}

Response Client::listRatings() {
    return performRequest(RequestType::LIST_RATINGS, { });
}

Response Client::showRating() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return Response::exitResponse();
    }
    return performRequest(RequestType::GET_RATING, { RequestField(id) });
}

Response Client::vote() {
    std::cout << "Enter rating id:" << std::endl;
    uint32_t id;
    if (!(std::cin >> id)) {
        return Response::exitResponse();
    }
    int option = 0;
    while (true) {
        std::cout << "Enter option index:" << std::endl;
        if (!(std::cin >> option)) {
            return Response::exitResponse();
        }
        if (0 <= option && option < 256) {
            break;
        }
        std::cerr << "Invalid option index. Must be in range [0..255]" << std::endl;
    }
    return performRequest(RequestType::VOTE, { RequestField(id), RequestField(static_cast<uint8_t>(option))});
}

Response Client::performRequest(Client::RequestType type, std::vector<RequestField> fields) {
    if (write(socketfd, &type, sizeof(type)) != sizeof(type)) {
        return Response::ResponseDisconnect();
    }
    for (auto &field : fields) {
        field.write(socketfd);
    }
    return Response::readResponse(reader);
}
