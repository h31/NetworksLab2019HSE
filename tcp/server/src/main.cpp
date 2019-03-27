#include "server.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    int port_number = (uint16_t) atoi(argv[1]);

    Server server = Server(port_number);

    if (!server.run()) {
        exit(1);
    }
}