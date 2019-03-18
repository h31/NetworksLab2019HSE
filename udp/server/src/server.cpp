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

    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
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
    this->computation_response_pool.clear();

    log("Server: destruction success.");
}

void server::wait_for_clients() {
    if (!this->isInitialized) {
        return;
    }

    while (!this->isTerminated) {
        dctp_request_header request{};
        if (socket_read_request(socket_descriptor, request, client_addr) < 0) {
            continue;
        }

        log("Server: new client connected. " + std::to_string(client_addr.sin_addr.s_addr));
        client_handler(request);
        log("Server: stopped work with client. " + std::to_string(client_addr.sin_addr.s_addr));
    }
}

void server::client_handler(struct dctp_request_header request) {
    struct dctp_response_header response{};
    if (computation_response_pool.contains(client_addr, request.id, request.type)) {
        response = computation_response_pool.get(client_addr, request.id, request.type);
    } else {
        response = handle_new_request(request);
        computation_response_pool.insert(client_addr, request.id, request.type, response);
    }

    socket_write_response(socket_descriptor, response, client_addr);
}

dctp_response_header server::handle_new_request(struct dctp_request_header request) {
    struct dctp_response_header response{};
    int64_t result;
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
            response = handle_slow_function(request, &server::process_fact);
            break;
        case SQRT:
            response = handle_slow_function(request, &server::process_sqrt);
            break;
        case LONG_COMPUTATION_RESULT:
            // If it's not in computation pool, then it doesn't exists.
            response = {UNKNOWN_OPERATION, SLOW, request.id, 0};
            break;
        default:
            response = {UNKNOWN_OPERATION, FAST, request.id, 0};
            break;
    }

    return response;
}

struct dctp_response_header server::handle_slow_function(
        struct dctp_request_header request,
        void(server::* process)(int, struct dctp_request_header)
        ) {
    struct dctp_response_header response{WAIT_FOR_RESULT, SLOW, request.id, 0};
    computation_response_pool.insert(client_addr, request.id, request_type::LONG_COMPUTATION_RESULT, response);

    std::thread* thread = new std::thread(process, this, socket_descriptor, request);
    slow_ops_pool.insert(socket_descriptor, request.id, thread);
    return response;
}

void server::process_sqrt(int socket_descriptor, struct dctp_request_header request) {
    sleep(3);
    struct dctp_response_header response{};
    if (request.first_operand < 0) {
        response = {SQRT_OF_NEGATIVE, SLOW, request.id, 0};
    } else {
        auto result = static_cast<int64_t>(sqrt(request.first_operand));
        response = {OK, SLOW, request.id, result};
    }

    computation_response_pool.insert(client_addr, request.id, request_type::LONG_COMPUTATION_RESULT, response);
    slow_ops_pool.remove(socket_descriptor, request.id);
}

void server::process_fact(int socket_descriptor, struct dctp_request_header request) {
    sleep(3);
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

    computation_response_pool.insert(client_addr, request.id, request_type::LONG_COMPUTATION_RESULT, response);
    slow_ops_pool.remove(socket_descriptor, request.id);
}

