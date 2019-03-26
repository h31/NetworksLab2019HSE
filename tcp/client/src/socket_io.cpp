#include <sys/socket.h>
#include "../include/socket_io.h"
#include "../include/network_exception.h"


void socket_io::read_bytes(char *bytes, size_t amount) {
    while (amount > 0) {
        ssize_t bytes_read = unistd::read(socket_fd, bytes, amount);
        if (bytes_read <= 0) {
            throw network_exception("Disconnected from server");
        }
        amount -= bytes_read;
        bytes += bytes_read;
    }
}

void socket_io::write_bytes(const char *bytes, size_t amount) {
    ssize_t result = unistd::write(socket_fd, bytes, amount);
    if (result <= 0) {
        throw network_exception("Disconnected from server");
    }
}

socket_io::socket_io(int socket_fd) : socket_fd(socket_fd) {}

std::string socket_io::read_string(size_t size) {
    char buffer[size];
    read_bytes(buffer, size);
    return std::string(buffer, buffer + size);
}

template<>
void socket_io::write<std::string>(const std::string &value) {
    write_bytes(value.c_str(), value.length());
}

void socket_io::close() {
    if (socket_fd != -1) {
        shutdown(socket_fd, SHUT_RDWR);
        socket_fd = -1;
    }
}

