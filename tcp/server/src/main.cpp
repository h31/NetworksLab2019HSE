#include <iostream>

#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

int main(int argc, char **argv) {
    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_descriptor < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct sockaddr_in server_addr{}, client_addr{};
    socklen_t client_size = sizeof(client_addr);

    bzero((char *) &server_addr, sizeof(server_addr));

    uint16_t port_number = 22229;
    if (argc > 1) {
        port_number = static_cast<uint16_t>(atoi(argv[1])); // NOLINT(cert-err34-c)
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    if (bind(socket_descriptor, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(socket_descriptor, 5);

    // something meaningless
    bool isTerminated = false;
    while (!isTerminated) {
        int new_socket_descriptor = accept(socket_descriptor, (struct sockaddr *) &client_addr, &client_size);
        if (new_socket_descriptor < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        isTerminated = true;
    }

    return 0;
}