#ifndef SERVER_SOCKET_READER_H
#define SERVER_SOCKET_READER_H

#include <stddef.h>
#include <string>
#include <unistd.h>

class SocketReader {

private:

    static const int BUFFER_SIZE = 1024;

    int socket_fd = 0;
    ssize_t buffer_position = 0;
    ssize_t buffer_size = 0;
    unsigned char buffer[BUFFER_SIZE];

    bool isEmpty();
    bool fill();

public:
    explicit SocketReader(int socket_fd);

    bool readBytes(void *destination, size_t size);
    bool readString(std::string &destination);
};

#endif
