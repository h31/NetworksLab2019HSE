//
// Created by Владислав Калинин on 11/02/2019.
//

#include "../include/ChatServer.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    ChatServer server(argv[1], atoi(argv[2]));
    server.run();
}

