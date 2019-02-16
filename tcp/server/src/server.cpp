#include "server.h"

#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

#include <thread>
#include <server.h>
#include <request.hpp>
#include <response.hpp>
#include <iostream>


server::server(uint16_t port_number) : port_number(port_number) {
    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_descriptor < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    if (bind(socket_descriptor, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
}

server::~server() {
    for (auto slow_operation : this->slow_ops_pool) {
        slow_operation->join();
    }

    close(socket_descriptor);

    for (auto socket_thread : this->socket_pool) {
        socket_thread->join();
    }
}

void server::start() {
    listen(socket_descriptor, 5);

    while (!this->isTerminated) {
        socklen_t client_size = sizeof(client_addr);
        int new_socket_descriptor = accept(socket_descriptor, (struct sockaddr *) &client_addr, &client_size);
        if (new_socket_descriptor < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        std::thread *thread = new std::thread(&server::client_handler, this, new_socket_descriptor);
        socket_pool.push_back(thread);
    }
}

void server::client_handler(int socket_descriptor) {
    while (true) {
        struct dctp_request_header request{};
        ssize_t c = read(socket_descriptor, &request, sizeof(request));

        if (c < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        struct dctp_response_header response{};
        std::cout << "kek\n";
        std::cout << request.type << " " << request.id << request.first_operand << request.second_operand << "\n";
        int64_t result;
        std::thread *thread;
        switch (request.type) {
            case PLUS:
                result = request.first_operand + request.second_operand;
                response = {OK, FAST, request.id, result};
                break;
            case MINUS:
                result = request.first_operand - request.second_operand;
                response = {OK, FAST, request.id, result};
                break;
            case MULT:
                result = request.first_operand * request.second_operand;
                response = {OK, FAST, request.id, result};
                break;
            case DIV:
                if (request.second_operand == 0) {
                    response = {DIV_BY_ZERO, FAST, request.id, 0};
                } else {
                    result = request.first_operand / request.second_operand;
                    response = {OK, FAST, request.id, result};
                }
                break;
            case FACT:
                response = {WAIT_FOR_RESULT, SLOW, request.id, 0};
                thread = new std::thread(&server::process_fact, this, socket_descriptor, request);
                slow_ops_pool.push_back(thread);
                break;
            case SQRT:
                response = {WAIT_FOR_RESULT, SLOW, request.id, 0};
                thread = new std::thread(&server::process_sqrt, this, socket_descriptor, request);
                slow_ops_pool.push_back(thread);
                break;
            default:break;
        }
        std::cout << response.return_code << " " << response.id << " " << response.result << "\n";

        c = write(socket_descriptor, &response, sizeof(response));
        std::cout << c << std::endl;
        std::cout << sizeof(response) << std::endl;
        if (c < 0) {
            perror("ERROR writing from socket");
            exit(1);
        }

    }
}

void server::process_sqrt(int socket_descriptor, struct dctp_request_header request) {

}

void server::process_fact(int socket_descriptor, struct dctp_request_header request) {

}



