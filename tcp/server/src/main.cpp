#include <iostream>

#include "server.h"

int main(int argc, char **argv) {
    uint16_t port_number = 22229;
    if (argc > 1) {
        port_number = static_cast<uint16_t>(atoi(argv[1])); // NOLINT(cert-err34-c)
    }

    server s(port_number);
    s.start();

    return 0;
}