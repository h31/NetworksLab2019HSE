#include "../include/server.h"

Server* server = NULL;

void handleCtrlC(int s) {
    printLog("Ctrl C cought!!");
    server->stop();
}

int main(int argc, char *argv[]) {
    uint16_t port;
    if (argc == 2) {
        port = static_cast<uint16_t>(std::stoi(std::string(argv[1])));
    } else {
        port = 8881;
    }
    server = new Server(port);

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = &handleCtrlC;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
    server->start();
    std::string s;
    while (true) {
        std::cin >> s;
        if (s == "kick") {

        }
    }
    return 0;
}