#include <iostream>
#include <unistd.h>
#include <thread>
#include "../include/BugTrackingServer.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid arguments number\n";
        return 0;
    }
    UserService userService;

    uint16_t port_number = static_cast<uint16_t>(std::stoi(argv[1]));

    std::cout << "Initializing server on port " << port_number << "\n";
    BugTrackingServer server(&userService);
    std::thread server_thread(&BugTrackingServer::initServer, &server, port_number);
    server_thread.detach();

    std::string in;
    int id;
    while (in != "exit") {
        std::cin >> in;
        if (in == "ban") {
            std::cin >> id;
            server.banClient(id);
        }
    }
    std::cout << "Shutting down server\n";
    close(port_number);
}