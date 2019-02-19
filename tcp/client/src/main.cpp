//
// Created by iisuslik on 17.02.19.
//

#include "../include/client.h"

int main(int argc, char *argv[]) {
    uint16_t port = 8881;
    std::string host = "localhost";
    if (argc >= 2) {
        host = std::string(argv[1]);
    }
    if (argc >= 3) {
        port = static_cast<uint16_t>(std::stoi(std::string(argv[2])));
    }
    Client client = Client(host, port);
    client.start();
}