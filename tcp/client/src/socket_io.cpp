#include "socket_io.h"
#include "socket_reading_exception.h"
#include "socket_writing_exception.h"

socket_io::socket_io(int socket_fd) : socket_fd(socket_fd) {}

void socket_io::read_bytes(char *bytes, size_t amount) {
    while (amount > 0) {
        ssize_t bytes_read = read(socket_fd, bytes, amount);
        if (bytes_read <= 0) {
            throw socket_reading_exception();
        }
        amount -= bytes_read;
        bytes += bytes_read;
    }
}

void socket_io::write_bytes(const char *bytes, size_t amount) {
    ssize_t result = write(socket_fd, bytes, amount);
    if (result <= 0) {
        throw socket_writing_exception();
    }
}

std::string socket_io::read_string(size_t size) {
    char buffer[size];
    read_bytes(buffer, size);
    return std::string(buffer, buffer + size);
}

template<>
void socket_io::write_data<std::string>(const std::string &value) {
    write_bytes(value.c_str(), value.length());
}