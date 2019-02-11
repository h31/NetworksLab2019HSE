#include <iostream>
#include <thread>
#include "../include/roulette_server.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./roulette_server <port number>";
        return 0;
    }

    uint16_t port_number = static_cast<uint16_t>(std::stoi(argv[1]));

    RouletteServer server;
    std::thread server_thread(&RouletteServer::StartServer, &server, port_number);
    server_thread.detach();

    std::string in;
    while (in != "exit") {
        std::cin >> in;
    }
    close(port_number);

    return 0;
}