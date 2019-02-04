#include <Server.h>

#define PORT 4567

int main(int argc, char const *argv[]) {
    vacancy::Server server(PORT);
    server.runServer();
}