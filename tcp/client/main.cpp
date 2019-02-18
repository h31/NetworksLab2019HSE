#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string>
#include <string>
#include <iostream>
#include "Client.h"

int main(int argc, char *argv[]) {


    if (argc < 3) {
        std::cerr << "usage %s hostname port\n", argv[0]
        exit(0);
    }

    uint16_t portno = (uint16_t) atoi(argv[2]);

    hostent* server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    try {
        Client client(server, portno);
        client.start();
    } catch (exception& e) {
        std::cerr << "Problems on opening socket and establishing connection\n";
    }
    return 0;
}