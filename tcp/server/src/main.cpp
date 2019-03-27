#include <iostream>
#include <thread>
#include "../include/market_server.h"

const std::string USAGE = "Usage: ./server <port number>";

const std::string CMDS = "exit -- close server\n"
                         "ban <name> -- force user disconnect";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << USAGE;
        return 0;
    }
    std::cout << CMDS << std::endl;

    uint16_t port_number = static_cast<uint16_t>(std::stoi(argv[1]));

    MarketServer server;
    std::thread server_thread(&MarketServer::StartServer, &server, port_number);
    server_thread.detach();

    std::string in;
    while (in != "exit") {
        std::cin >> in;
        if (in == "ban") {
            std::string name;
            std::cin >> name;
            if (server.BanUser(name)) {
                std::cout << "A player was successfully banned. Good job!\n";
            } else {
                std::cout << name << " doesn't play at this moment.\n";
            }
        }
    }
    close(port_number);

    return 0;
}