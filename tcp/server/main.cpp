#include <iostream>
#include <cstdint>

#include "./include/server.h"

int main(int argc, char **argv)
{
    uint16_t port = 23923;

    if (argc > 1)
        port = std::stoi(argv[1]);

    std::cout << "Server started, port " << port << std::endl;

    Server server(port);

    std::cout << "Server stoped" << std::endl;
    server.Run();
}
