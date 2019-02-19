#include "client.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    uint16_t portno = (uint16_t) atoi(argv[2]);

    
    Client client(argv[1], portno);
    client.run();

    return 0;
}