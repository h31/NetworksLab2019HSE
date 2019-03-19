#include <signal.h>
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"

static server *serv = nullptr;

void term_handler(int signum) {
    shutdown(serv->get_socket_fd(), SHUT_RD);
    close(serv->get_socket_fd());
    serv->done = 1;
}

void server_main_loop(uint16_t port) {
    server payment_system_server = server(port);
    serv = &payment_system_server;
    payment_system_server.start();
}

int main(int argc, char **argv) {
    signal(SIGINT, term_handler);
    signal(SIGTERM, term_handler);

    if (argc != 2) {
        std::cerr << "This command accepts only 1 argument: server port.\n";
        return 1;
    }

    int port = atoi(argv[1]);

    if (port > 0 && port < (1 << 16)) {
        server_main_loop(static_cast<uint16_t>(port));
    } else {
        std::cerr << "Port has to be 1-65535.\n";
        return 1;
    }

    return 0;
}
