#include <thread>
#include <mutex>

#include "Server.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: server <PORT>" << std::endl;
        return 0;
    }
    Server server(static_cast<unsigned short>(strtol(argv[1], nullptr, 10)));
    server.Run();
    return 0;
}
