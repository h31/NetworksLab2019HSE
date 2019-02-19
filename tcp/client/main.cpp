#include <iostream>
#include <string>

#include "./include/client.h";

std::string USAGE = "Please provide exactly two arguments in the same order: hostname and port.";

int main (int argc, char* argv[]) {
    if (argc < 3)
        std::cerr << "Incorrect number of arguments." << std::endl << USAGE;

    std::string hostname = std::string(argv[1]);
    uint16_t port = (uint16_t) std::atoi(argv[2]);
    Client* client = new Client(hostname, port);
    std::string cmd, message;
    int id;

    bool exit = false;

    std::cout << "After the client launch, enter message receiver id (or 0 to send message to everyone) and the message"
              << std::endl << "Or enter \'exit\' to exit";

    while (!client -> Start()) {
        std::cout << "Error during the connection." 
                  << "Please enter \'exit\' to exit or any other not empty command to try one more time" 
                  << std::endl << "> ";

        std::cin >> cmd;

        if (cmd == "exit") {
            exit = true;
            break;
        }
    }

    while (!exit) {
        std::cin >> cmd;
        if (cmd == "exit")
            break;
        try {
            id = std::stoi(cmd);
        } catch (...) {
            continue;
        }
        std::cin >> message;
        if (id)
            client -> SendMessageToOne(id, message);
        else
            client -> SendMessageToAll(message);
    }

    delete client;
    return 0;
}