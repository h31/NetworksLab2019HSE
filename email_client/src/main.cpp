#include "client.h"

namespace {
    std::function<void(int)> shutdown_handler;

    void signal_handler(int signal) {
        shutdown_handler(signal);
    }
}

int main() {
    auto client = email::Client::from_input();
//    auto client = email::Client("aaa@gmail.com", "127.0.0.1" , 1111);
    shutdown_handler = [&](int d) {
        std::cout << "catch signal: " << d << std::endl;
        client.shut_down();
        exit(0);
    };

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    client.run();
    return 0;
}