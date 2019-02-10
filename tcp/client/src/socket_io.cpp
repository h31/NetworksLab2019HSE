#include "socket_io.h"

#include "disconnected_server_exception.h"


void socket_io::read_bytes(char *bytes, size_t amount) {
    bzero(bytes, amount);
    ssize_t result = read(socket_fd, bytes, amount);
    if (result < 0) {
        throw socket_reading_exception();
    }
    if (result == 0) {
        throw disconnected_server_exception();
    }
}

void socket_io::write_bytes(const char *bytes, size_t amount) {
    ssize_t result = write(socket_fd, bytes, amount);
    if (result < 0) {
        throw socket_writing_exception();
    }
    if (result == 0) {
        throw disconnected_server_exception();
    }
}

socket_io::socket_io(int socket_fd) : socket_fd(socket_fd) {}

int socket_io::read_int() {
    char buffer[sizeof(int)];
    read_bytes(buffer, sizeof(int));
    return boost::lexical_cast<int>(buffer);
}

size_t socket_io::read_size_t() {
    char buffer[sizeof(size_t)];
    read_bytes(buffer, sizeof(size_t));
    return boost::lexical_cast<size_t>(buffer);
}

std::string socket_io::read_string(size_t size) {
    char buffer[size];
    read_bytes(buffer, size);
    return std::string(buffer, buffer + size);
}

void socket_io::write_int(int n) {
    auto string = boost::lexical_cast<std::string>(n);
    write_bytes(string.c_str(), sizeof(int));
}

void socket_io::write_size_t(size_t n) {
    auto string = boost::lexical_cast<std::string>(n);
    write_bytes(string.c_str(), sizeof(size_t));
}

void socket_io::write_string(const std::string &string) {
    write_bytes(string.c_str(), string.length());
}

