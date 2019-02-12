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

struct Message {
    uint64_t timestamp;
    uint32_t sender_len;
    const char* sender;
    uint32_t message_len;
    const char* message;

    ~Message() {
        delete [] sender;
        delete [] message;
    }
};

class Client {
public:
    static Client create_client(const char *hostname, uint16_t port, const char *nickname);

    void send_message(size_t reciever_length, char* reciever, size_t message_length, char* message);
    bool has_new_message();
    void read_message();
    void disconnect();

    ~Client() {};
private:
    Client(int sockfd, const Protocol::ClientHeader header);

    int sockfd = -1;
    std::set<std::pair<std::string, int64_t>> processing_messages;
    Protocol::ClientHeader client_header;
};


#endif //CLIENT_CLIENT_H
