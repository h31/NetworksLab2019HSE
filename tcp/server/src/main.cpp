#include <iostream>
#include <csignal>

#include "server.h"

server* s_pointer;
void signal_handler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";

    delete s_pointer;
    exit(signum);
}

int main(int argc, char **argv) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    uint16_t port_number = 22229;
    if (argc > 1) {
        port_number = static_cast<uint16_t>(atoi(argv[1])); // NOLINT(cert-err34-c)
    }

    s_pointer = new server(port_number);
    s_pointer->start();

    return 0;
}