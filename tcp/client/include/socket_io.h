#ifndef SERVER_SOCKETIO_H
#define SERVER_SOCKETIO_H

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string>
#include <strings.h>
#include <boost/lexical_cast.hpp>


class socket_io {
    private:
        int socket_fd;

        void read_bytes(char *bytes, size_t amount);

        void write_bytes(const char *bytes, size_t amount);

    public:
        explicit socket_io(int socket_fd);

        int read_int();

        size_t read_size_t();

        std::string read_string(size_t size);

        void write_int(int n);

        void write_size_t(size_t n);

        void write_string(const std::string &string);

        void close();
};

#endif //SERVER_SOCKETIO_H
