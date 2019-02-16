//
// Created by mikhail on 03.02.19.
//

#include <PrimesServerApplication.h>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "usage: .%s <port (optional)>\n", argv[0]);
        return 0;
    }
    if (argc == 2) {
        auto portno = static_cast<uint16_t>(strtol(argv[1], nullptr, 10));
        PrimesServerApplication(portno).run();
    } else {
        PrimesServerApplication().run();
    }

    return 0;
}

void PrimesServerApplication::run() {
    printUsage();

    std::thread server_thread(&Server::run, std::ref(server));
    bool toExit = false;
    while (!toExit) {
        std::string command;
        std::cin >> command;
        if (command == "shutdown") {
            uint32_t client_id;
            std::cin >> client_id;
            if (server.shutdown_client(client_id)) {
                std::cout << "Client was disconnected successfully\n";
            } else {
                std::cout << "No such client\n";
            }
        } else if (command == "exit") {
            server.stop();
            toExit = true;
        } else {
            std::cout << "UNKNOWN COMMAND\n";
        }
    }
    server_thread.join();
}

void PrimesServerApplication::printUsage() {
    std::cout << "this server runs on " << portno << " port\n"
    << "Commands:\n"
    << "shutdown <client_id> - close connection with specific client\n"
    << "exit - close server\n"
    << "\n";
}

PrimesServerApplication::PrimesServerApplication(uint16_t portno) : portno(portno), server(portno, 10) {}

PrimesServerApplication::PrimesServerApplication() : PrimesServerApplication(PrimesServerApplication::DEFAULT_PORTNO) {}