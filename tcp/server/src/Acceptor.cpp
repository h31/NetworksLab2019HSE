#include <netinet/in.h>
#include <cstdio>
#include <thread>
#include <vector>

#include "Acceptor.h"
#include "Worker.h"

void Acceptor::operator()(Server *server) {
    std::vector<std::thread> threads;
    while (true) {
        int client_socket_fd = server->Accept();
        if (client_socket_fd < 0) {
            break;
        }
        auto client = new Client(client_socket_fd);
        std::thread client_thread(Worker(), server, client);
        threads.push_back(std::move(client_thread));
    }
    perror("Error on accept");
    server->ShutdownAllClients();
    for (auto &thread : threads) {
        thread.join();
    }
}
