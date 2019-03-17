#include "server.h"

namespace {
    std::function<void(int)> shutdown_handler;

    void signal_handler(int signal) {
        shutdown_handler(signal);
    }
}

int main() {
    uint16_t port = vacancy_service::Server::DEFAULT_PORT;
    std::string port_str = util::UserView::get_user_input(
        "Enter server port or leave default " + std::to_string(port) + ": ");

    port = port_str.empty() ? port : static_cast<uint16_t>(std::stoi(port_str));

    vacancy_service::Server server(port);

    shutdown_handler = [&](int) {
        server.shut_down();
        exit(0);
    };
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    server.run();
    return 0;
}