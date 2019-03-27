#ifndef SERVER_SOCKETIO_H
#define SERVER_SOCKETIO_H

#include <string>
#include <unistd.h>

class socket_io {
private:
    int socket_fd;

    void read_bytes(char *bytes, size_t amount);

    void write_bytes(const char *bytes, size_t amount);

public:
    explicit socket_io(int socket_fd);

    std::string read_string(std::size_t size);

    template<class T>
    T read_data() {
        char buffer[sizeof(T)];
        read_bytes(buffer, sizeof(T));
        T result;
        memcpy(&result, buffer, sizeof(T));
        return result;
    }

    template<class T>
    void write_data(const T &value) {
        write_bytes(static_cast<const char *>(static_cast<const void *>(&value)), sizeof(T));
    }

    void close();
};

#endif //SERVER_SOCKETIO_H