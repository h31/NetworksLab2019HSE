
#include <io_util.h>

#include "io_util.h"

void write_to_socket(int socket_descriptor, const void *buf, size_t size) {
    ssize_t n = write(socket_descriptor, buf, size);
    if (n < 0) {
        error("write", "failed to write to socket!");
    }

    if (n != size) {
        error("write", "unexpected end of socket! (Maybe server disconnect?)");
    }
}

void read_from_socket(int socket_descriptor, void *buf, size_t size) {
    ssize_t n = read(socket_descriptor, buf, size);
    if (n < 0) {
        error("read", "failed to read from socket!");
    }

    if (n != size) {
        error("read", "unexpected end of socket! (Maybe server disconnect?)");
    }
}


void print(const std::string &s) {
    std::cout << s;
}

void println(const std::string &s) {
    print(s + "\n");
}

void error(const std::string &s) {
    println("Error: " + s);
    exit(1);
}

void error(const std::string &type, const std::string &s) {
    error(type + ": " + s);
}

pstp_response_header read_header(int socket_descriptor) {
    struct pstp_response_header response_header;
    read_from_socket(socket_descriptor, (char *) &response_header, sizeof(response_header));

    if (response_header.code == INVALID_PASSWORD) {
        error("read header", "invalid combination of login-password!");
    }

    return response_header;
}

std::string read_until_zero(int* ptr, char* buffer, size_t buffer_size) {
    std::string dest;
    while (*ptr < buffer_size && buffer[*ptr] != '\0') {
        dest += buffer[*ptr];
        (*ptr)++;
    }
    (*ptr)++;

    return dest;
}

