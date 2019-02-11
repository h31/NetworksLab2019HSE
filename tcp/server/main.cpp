#include <iostream>
#include "Server.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        return 1;
    }
    std::string root_dir = std::string(argv[1]);
    Server server(root_dir);
    server.start(std::stoi(argv[2]));
    server.listenSoket();
}