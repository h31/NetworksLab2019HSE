//
// Created by mikhail on 16.02.19.
//

#include <string>
#include <string.h>
#include <zconf.h>
#include <PrimeNumbersConcurrent.h>
#include <sys/socket.h>
#include <iostream>
#include "ClientHandler.h"

void check_connection_status(ssize_t read_bytes_number) {
    if (read_bytes_number < 0) {
        std::string message = "ERROR communicating with socket.";
        perror(message.c_str());
        throw std::string(message);
    }

    if (read_bytes_number == 0) {
        throw "socket closed";
    }
}

int64_t read_int64(int sockfd) {
    int64_t int_value = 0;
    check_connection_status(read(sockfd, &int_value, sizeof(int_value)));
    return int_value;
}

int32_t read_int32(int sockfd) {
    int32_t int_value = 0;
    check_connection_status(read(sockfd, &int_value, sizeof(int_value)));
    return int_value;
}

int16_t read_int16(int sockfd) {
    int16_t int_value = 0;
    check_connection_status(read(sockfd, &int_value, sizeof(int_value)));
    return int_value;
}


void write_int64(int64_t value, int sockfd) {
    check_connection_status(write(sockfd, &value, sizeof(int64_t)));
}

void write_int8(int8_t value, int sockfd) {
    check_connection_status(write(sockfd, &value, sizeof(int8_t)));
}

int16_t ClientHandler::read_message_end() const {
    return read_int16(client_sockfd);
}

ClientHandler::ClientHandler(int client_sockfd, PrimeNumbersConcurrent &prime_numbers) :
        client_sockfd(client_sockfd), prime_numbers(prime_numbers) {}

void ClientHandler::run() const {
    bool is_stopped = false;
    while (!is_stopped) {
        try {
            int32_t command = read_command();

            switch (command) {
                case 0:
                    process_max_number();
                    break;
                case 1:
                    process_last_numbers();
                    break;
                case 2:
                    process_bound_for_calculation();
                    break;
                case 3:
                    process_add_prime_numbers();
                    break;
                default:
                    perror("Corrupted message");
                    break;
            }
        } catch (const char *exception_to_stop) {
            is_stopped = true;
        }
    }
}

int32_t ClientHandler::read_command() const {
    return read_int32(client_sockfd);
}

void ClientHandler::process_max_number() const {
    read_message_end();
    write_int64(prime_numbers.get_max(), client_sockfd);
    write_message_end();
}

void ClientHandler::process_last_numbers() const {
    int64_t primes_number_to_return = read_int64(client_sockfd);
    read_message_end();
    std::vector<int64_t> numbers_to_return = prime_numbers.get_last((size_t) primes_number_to_return);
    write_int64(numbers_to_return.size(), client_sockfd);
    for (int64_t prime_number : numbers_to_return) {
        write_int64(prime_number, client_sockfd);
    }
    write_message_end();
}

void ClientHandler::process_bound_for_calculation() const {
    read_int64(client_sockfd);
    read_message_end();
    write_int64(prime_numbers.get_bound_for_calculation(), client_sockfd);
    write_message_end();
}

void ClientHandler::process_add_prime_numbers() const {
    int64_t n = read_int64(client_sockfd);
    std::vector<int64_t> numbers_to_add;
    for (int64_t i = 0; i < n; i++) {
        numbers_to_add.push_back(read_int64(client_sockfd));
    }
    read_message_end();
    write_int8(prime_numbers.add_prime_numbers(numbers_to_add), client_sockfd);
    write_message_end();
}

void ClientHandler::write_message_end() const {
    write_int8('\\', client_sockfd);
    write_int8(0, client_sockfd);
}
