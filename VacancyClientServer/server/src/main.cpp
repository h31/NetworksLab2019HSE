#include <Server.h>
#include <csignal>
#include <iostream>
#include <sstream>

#define PORT 4567

namespace {
    std::function<void(int)> shutdown_handler;
    void signal_handler(int signal) { shutdown_handler(signal); }
}

int main(int argc, char const *argv[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    uint16_t port = PORT;
    std::cout << "Enter port (or leave empty for " << PORT << ") :> ";
    std::string input;
    getline(std::cin, input);
    if (!input.empty()) {
        std::istringstream stream( input );
        stream >> port;
    }

    vacancy::Server server(port);
    shutdown_handler = [&](int signal) {
        std::cout << "Server shutdown...\n";
        server.shutdown();
    };
    server.runServer();
}