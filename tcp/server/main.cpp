#include <iostream>
#include <csignal>
#include "server.h"

std::function<void(int)> shutdown_handler;
void signal_handler(int signal) {
    shutdown_handler(signal);
}

int main() {
    try {
        Server server(5003);
        shutdown_handler = [&server](int s) {
            server.cancel();
            std::cout << "Shutting down" << std::endl;
            exit(0);
        };
        signal(SIGINT, signal_handler);
        server.start();
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }
    return 0;
}

