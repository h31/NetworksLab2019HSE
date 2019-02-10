#include <signal.h>
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"

void server_main_loop() {
    server payment_system_server = server(1337);
    payment_system_server.start();
}

int main() {
    signal(SIGINT, term_handler);
    signal(SIGTERM, term_handler);

    server_main_loop();

    return 0;
}
