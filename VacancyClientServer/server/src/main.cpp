#include <Server.h>
#include <csignal>
#include <iostream>

#define PORT 4567

namespace {
    std::function<void(int)> shutdown_handler;
    void signal_handler(int signal) { shutdown_handler(signal); }
}

int main(int argc, char const *argv[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    vacancy::Server server(PORT);
    shutdown_handler = [&](int signal) {
        std::cout << "Server shutdown...\n";
        server.shutdown();
    };
    server.runServer();
}