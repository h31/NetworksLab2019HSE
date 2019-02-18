#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string>
#include <string>
#include <iostream>
#include <exception>
#include "client.h"

int main(int argc, char *argv[]) {


    if (argc < 3) {
        std::cerr << "usage " << argv[0] << " hostname port\n";
        return 0;
    }

    uint16_t portno = (uint16_t) atoi(argv[2]);

    hostent* server = gethostbyname(argv[1]);

    if (server == NULL) {
        std::cerr << "ERROR, no such host\n";
        return 0;
    }
    try {
        Client client(server, portno);
        client.start();
    } catch (std::exception& e) {
        std::cerr << "Problems on opening socket and establishing connection\n";
    }
    return 0;
}
