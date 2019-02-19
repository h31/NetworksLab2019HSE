#include <iostream>
#include <thread>
#include "../include/roulette_server.h"

const std::string USAGE = "Usage: ./roulette_server <port number>";

const std::string CMDS = "exit -- close server\n"
                         "ban <name> -- to disconnect a player from game";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << USAGE;
        return 0;
    }
    std::cout << CMDS << std::endl;

    uint16_t port_number = static_cast<uint16_t>(std::stoi(argv[1]));

    RouletteServer server;
    std::thread server_thread(&RouletteServer::StartServer, &server, port_number);
    server_thread.detach();

    std::string in;
    while (in != "exit") {
        std::cin >> in;
        if (in == "ban") {
            std::string name;
            std::cin >> name;
            if (server.BanPlayer(name)) {
                std::cout << "A player was successfully banned. Good job!\n";
            } else {
                std::cout << name << " doesn't play at this moment.\n";
            }
        }
    }
    shutdown(port_number, SHUT_RDWR);

    return 0;
}