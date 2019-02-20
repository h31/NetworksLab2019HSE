//
// Created by Olga Alehina on 2019-02-18.
//
#include <iostream>
#include "../include/Client.h"
int main(int argc, char *argv[]) {

    Client client;
    char* host;
    uint16_t port;
    if (argc < 3)  {
        std::cout << "Введите хост: ";
        std::cin >> host;
        std::cout << "Введите порт: ";
        std::cin >> port;
    }
    else {
        host = argv[1];
        port = (uint16_t) atoi(argv[2]);
    }
    client.start(host, port);
}

