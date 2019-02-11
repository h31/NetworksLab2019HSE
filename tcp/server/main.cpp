#include <iostream>
#include "Server.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }
    std::string root_dir = std::string(argv[1]);
    Server server(root_dir);
    server.start(5050);
    while (true) {
        server.listenSoket();
    }
}