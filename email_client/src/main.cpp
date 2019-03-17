#include "client.h"

namespace {
    std::function<void(int)> shutdown_handler;

    void signal_handler(int signal) {
        shutdown_handler(signal);
    }
}

int main() {
    auto client = email::Client::from_input();

    shutdown_handler = [&](int) {
        client.shut_down();
        exit(0);
    };
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    client.run();
    return 0;
}