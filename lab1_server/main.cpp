#include "server.h"

int main(int argc, char *argv[]) {
    Server server = Server(5001);
    server.run();

    return 0;
}
