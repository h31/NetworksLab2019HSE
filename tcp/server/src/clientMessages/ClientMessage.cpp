#include <unistd.h>
#include <netinet/in.h>

#include "clientMessages/ClientMessage.h"

void ClientMessage::ReadString(int socket_fd, std::string &dst) {
    unsigned char next_char;
    do {
        read(socket_fd, &next_char, sizeof(next_char));
        if (next_char == 0) {
            break;
        }
        dst += next_char;
    } while (next_char != 0);
}
