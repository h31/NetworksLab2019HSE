#include <thread>
#include <mutex>

#include "Server.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: server <PORT>" << std::endl;
        return 0;
    }
    auto port = static_cast<unsigned short>(strtol(argv[1], nullptr, 10));
    if (port < 1024) {
        std::cerr << "Invalid port number: must be 1024-65535" << std::endl;
    }
    Server server(port);
    server.Run();
    return 0;
}
