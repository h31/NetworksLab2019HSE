#include <netinet/in.h>
#include <cstdio>
#include <thread>

#include "Acceptor.h"
#include "Worker.h"

void Acceptor::operator()(Server *server) {
    while (true) {
        struct sockaddr_in client_address{};
        unsigned int client_length = sizeof(client_address);
        int client_socket_fd = accept(server->GetSocket(), (struct sockaddr *) &client_address, &client_length);
        if (client_socket_fd < 0) {
            perror("Error on accept");
            server->ShutdownAllClients();
            break;
        }
        auto client = new Client(client_socket_fd);
        std::thread client_thread(Worker(), server, client);
        client->SetThread(&client_thread);
    }
}
