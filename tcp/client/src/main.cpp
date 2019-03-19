#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <request.hpp>
#include <response.hpp>
#include "login.h"
#include "io_util.h"
#include "main_cycle.h"

int connect(std::string& hostname, uint16_t port_number) {
    int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor < 0) {
        error("Failed to open the magic gates to Internet!");
    }

    struct hostent *server = gethostbyname(hostname.c_str());

    if (server == nullptr) {
        error("no such host");
    }

    struct sockaddr_in server_addr{};
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &server_addr.sin_addr.s_addr, (size_t) server->h_length);
    server_addr.sin_port = htons(port_number);

    if (connect(socket_descriptor, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("connection failed (for some reason)!");
    }

    return socket_descriptor;
}

Identifier get_identifier(int socket_descriptor) {
    std::string answer = has_account();
    if (answer == "y") {
        return login(socket_descriptor);
    } else if (answer == "n") {
        return registration(socket_descriptor);
    } else {
        error("unknown answer!");
    }
}

int main(int argc, char *argv[]) {
    std::cout << "Hello!\n";

    if (argc < 3) {
        error("hostname and port number are required!");
    }

    println("Establishing connection...");

    std::string hostname = argv[1];
    auto port_number = (uint16_t) atoi(argv[2]); // NOLINT(cert-err34-c)

    int socket_descriptor = connect(hostname, port_number);

    println("Connection established!");

    Identifier ident = get_identifier(socket_descriptor);
    println("Welcome, user " + ident.login + "!");

    println("");

    print_help();
    main_cycle(ident, socket_descriptor);

    return 0;
}