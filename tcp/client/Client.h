#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <set>
#include "protocol.h"
#include "IncomingEvent.h"

class Client {
public:
    static Client create_client(const char *hostname, uint16_t port, const char *nickname);

    void send_message(size_t reciever_length, const char* reciever, size_t message_length, const char* message);
    IncomingEvent get_incoming_event();
    void disconnect();
    void shut_down();
private:
    Client(int sockfd, Protocol::ClientHeader header);

    int sockfd = -1;
    std::set<std::pair<std::string, int64_t>> processing_messages;
    Protocol::ClientHeader client_header;

    std::string read_message();
    std::string read_confirmation();
    void write_confirmation(int32_t username_length, const char *username, int64_t now);
};


#endif //CLIENT_CLIENT_H
