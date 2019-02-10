#include "util.h"

void write_to_socket(int socket_descriptor, const void *buf, size_t size) {
    ssize_t n = write(socket_descriptor, buf, size);
    if (n < 0) {
        error("failed to write to socket!");
    }
}

void read_from_socket(int socket_descriptor, void *buf, size_t size) {
    ssize_t n = read(socket_descriptor, buf, size);
    if (n < 0) {
        error("failed to read from socket!");
        exit(0);
    }

    if (n == 0) {
        error("end of socket! (Maybe server disconnect?)");
        exit(0);
    }
}

void println(const std::string &s) {
    std::cout << s << "\n";
}

void error(const std::string &s) {
    println("Error: " + s);
    exit(0);
}