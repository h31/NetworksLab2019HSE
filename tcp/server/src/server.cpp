#include "server.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

#include <cmath>
#include <thread>
#include <server.h>
#include <request.hpp>
#include <response.hpp>
#include <iostream>

#include <util.hpp>


server::server(uint16_t port_number) : port_number(port_number) {
    log("Server: initialization begin.");

    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor < 0) {
        log_error("can't open socket.");
        return;
    }
    int k = 1;
    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &k, sizeof(int)) < 0) {
        log_error("setsockopt(SO_REUSEADDR) failed");
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    if (bind(socket_descriptor, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        log_error("can't bind.");
        return;
    }
    isInitialized = true;

    log("Server: initialization success.");
}

server::~server() {
    log("Server: destruction begin.");
    this->isTerminated = true;
    close(socket_descriptor);

    this->slow_ops_pool.clear();
    this->client_socket_pool.clear();

    log("Server: destruction success.");
}

void server::wait_for_clients() {
    if (!this->isInitialized) {
        return;
    }
    listen(socket_descriptor, 5);

    while (!this->isTerminated) {
        socklen_t client_size = sizeof(client_addr);
        int new_socket_descriptor = accept(socket_descriptor, (struct sockaddr *) &client_addr, &client_size);
        if (new_socket_descriptor < 0) {
            log_error("failed to accept");
            continue;
        }
        log("Server: new socket connected. " + std::to_string(new_socket_descriptor));
        std::thread *thread = new std::thread(&server::client_handler, this, new_socket_descriptor);
        client_socket_pool.insert(new_socket_descriptor, thread);
    }
}

void server::client_handler(int socket_descriptor) {
    while (!this->isTerminated) {
        struct dctp_request_header request{};
        if (socket_read_request(socket_descriptor, request) < 0) {
            break;
        }

        struct dctp_response_header response{};
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
                slow_ops_pool.insert(socket_descriptor, request.id, thread);
                break;
            case SQRT:
                response = {WAIT_FOR_RESULT, SLOW, request.id, 0};
                thread = new std::thread(&server::process_sqrt, this, socket_descriptor, request);
                slow_ops_pool.insert(socket_descriptor, request.id, thread);
                break;
            default:
                response = {UNKNOWN_OPERATION, FAST, request.id, 0};
                break;
        }
        if (socket_write_response(socket_descriptor, response) < 0) {
            break;
        }
    }

    client_socket_pool.remove(socket_descriptor);
    close(socket_descriptor);
    log("Server: closed socket " + std::to_string(socket_descriptor));
}

void server::process_sqrt(int socket_descriptor, struct dctp_request_header request) {
    sleep(2);
    struct dctp_response_header response{};
    if (request.first_operand < 0) {
        response = {SQRT_OF_NEGATIVE, SLOW, request.id, 0};
    } else {
        auto result = static_cast<int64_t>(sqrt(request.first_operand));
        response = {OK, SLOW, request.id, result};
    }

    socket_write_response(socket_descriptor, response);
    slow_ops_pool.remove(socket_descriptor, request.id);
}

void server::process_fact(int socket_descriptor, struct dctp_request_header request) {
    sleep(2);
    struct dctp_response_header response{};
    if (request.first_operand < 0) {
        response = {FACT_OF_NEGATIVE, SLOW, request.id, 0};
    } else if (request.first_operand > 20) {
        response = {OVERFLOW, SLOW, request.id, 0};
    } else {
        int64_t result = 1;
        for (int i = 1; i <= request.first_operand; i++) {
            result *= i;
        }
        response = {OK, SLOW, request.id, result};
    }

    socket_write_response(socket_descriptor, response);
    slow_ops_pool.remove(socket_descriptor, request.id);
}

