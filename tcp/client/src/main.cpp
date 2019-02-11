#include "client.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    int port_number = (uint16_t) atoi(argv[2]);

    Client client = Client(argv[1], port_number);

    if (!client.connect()) {
        perror("ERROR opening socket");
        exit(1);
    }

    while(client.sendRequest()) {
        client.getResponse();
    }
}