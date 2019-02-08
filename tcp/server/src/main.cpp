#include <iostream>
#include "server.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./roulette_server <port number>";
        return 0;
    }

    uint16_t port_number = static_cast<uint16_t>(std::stoi(argv[1]));

    RouletteServer server;
    server.StartServer(port_number);

    return 0;
}