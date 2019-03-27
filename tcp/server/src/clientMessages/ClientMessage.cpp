#include <unistd.h>
#include <netinet/in.h>

#include "clientMessages/ClientMessage.h"

bool ClientMessage::ReadString(int socket_fd, std::string &dst) {
    unsigned char next_char;
    do {
        if (read(socket_fd, &next_char, sizeof(next_char)) < sizeof(next_char)) {
            return false;
        }
        if (next_char == 0) {
            break;
        }
        dst += next_char;
    } while (next_char != 0);
    return true;
}
