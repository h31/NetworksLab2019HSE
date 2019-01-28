#include "../include/Server.h"

#define PORT 4567

int main(int argc, char const *argv[]) {
    Server server(PORT);
    server.run_server();
}

