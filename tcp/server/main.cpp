
#include <iostream>

#include "./include/server.h"

int main(int argc, char **argv)
{
    uint16_t port = 23923;

    if (argc > 1)
        port = std::stoi(argv[1]);

    std::cout << "Server started, port " << port << std::endl;

    Server server(port);

    std::thread server_thread(&Server::Run, &server);
    server_thread.detach();

    std::cout << "Enter \'stop\' to stop." << std::endl << "$ ";
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "stop")
            break;
        std::cout << cmd << std::endl << "$ ";
    }

    std::cout << "Server stoped" << std::endl;
    return 0;
}