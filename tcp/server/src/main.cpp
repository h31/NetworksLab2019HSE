#include <iostream>
#include "server.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Specify port number. Usage: ./server [port]";
        return 0;
    }

    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));
    server server(port);
    server.start();

    std::string input;
    while (input != "stop") {
        std::cout << "Enter \"stop\" to stop the server" << std::endl;
        std::getline (std::cin, input);
    }
    server.stop();
}