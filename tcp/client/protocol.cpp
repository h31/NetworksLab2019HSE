#include "protocol.h"

#include <unistd.h>
#include <iostream>


namespace Protocol {
    ClientHeader::ClientHeader(uint32_t type, const char* username) : type(type) {
        size_t username_size = strlen(username);
        this->username = new char[username_size];
        bcopy(username, this->username, username_size);
        username_length = static_cast<int32_t>(username_size);
    };

    ClientHeader::~ClientHeader() {
        delete [] this->username;
    }

    void ClientHeader::write_to_fd(int fd, int32_t type) {
        write(fd, (char *) &type, sizeof(type));
        write(fd, (char *) &username_length, sizeof(username_length));
        write(fd, username, username_length);
    }
};