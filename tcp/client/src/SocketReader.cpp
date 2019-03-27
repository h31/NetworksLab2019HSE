#include <cstring>
#include "SocketReader.h"

SocketReader::SocketReader(int socket_fd) : socket_fd(socket_fd) {}

bool SocketReader::readBytes(void *destination, size_t size) {
    auto remaining_size = static_cast<ssize_t>(size);
    while (remaining_size > 0) {
        if (isEmpty() && !fill()) {
            return false;
        }
        auto copy_size = static_cast<size_t>(std::min(remaining_size, buffer_size - buffer_position));
        memcpy(destination, buffer + buffer_position, copy_size);
        remaining_size -= copy_size;
        buffer_position += copy_size;
        destination = static_cast<char *>(destination) + copy_size;
    }
    return true;
}

bool SocketReader::readString(std::string &destination) {
    unsigned char next_char;
    do {
        if (isEmpty() && !fill()) {
            return false;
        }
        next_char = buffer[buffer_position++];
        destination += next_char;
    } while (next_char != 0);
    destination.pop_back();
    return true;
}

bool SocketReader::isEmpty() {
    return buffer_position >= buffer_size;
}

bool SocketReader::fill() {
    buffer_size = read(socket_fd, buffer, BUFFER_SIZE);
    buffer_position = 0;
    return !isEmpty();
}
