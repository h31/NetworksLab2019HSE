#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include "Client.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: hostname, port, username");
        exit(0);
    }
    Client::create_client(argv[1], static_cast<uint16_t>(atoi(argv[2])), argv[3]);
}