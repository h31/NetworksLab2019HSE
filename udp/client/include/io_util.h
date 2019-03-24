#ifndef CLIENT_UTIL_H
#define CLIENT_UTIL_H

#include <iostream>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <response.hpp>
#include "client_exception.h"

void write_to_socket(int socket_descriptor, const void *buf, size_t size);

void read_from_socket(int socket_descriptor, void *buf, size_t size);

void print(const std::string &s);

void println(const std::string &s);

void error(const std::string &s);

void error(const std::string &type, const std::string &s);

pstp_response_header read_header(int socket_descriptor);

template <typename T>
T read_thing(int socket_descriptor) {
    T thing;
    read_from_socket(socket_descriptor, (char *) &thing, sizeof(T));

    return thing;
}

std::string read_until_zero(int* ptr, char* buffer);

char* read_bytes(int socket_descriptor, size_t size);

void read_string_pair_vector(char *buffer, uint32_t num_of_elements, std::vector<std::pair<std::string, std::string>>& dest);

#endif //CLIENT_UTIL_H
