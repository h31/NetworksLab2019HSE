#include <iostream>
#include "message.h"

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>


//int main(int argc, char* argv[]) {
//    int sockfd, n;
//    uint16_t portno;
//    sockaddr_in serv_addr{};
//    hostent* server;
//
//    char buffer[256];
//
//    if (argc < 3) {
//        fprintf(stderr, "usage %s hostname port\n", argv[0]);
//        exit(0);
//    }
//
//    portno = (uint16_t) atoi(argv[2]);
//
//    /* Create a socket point */
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//
//    if (sockfd < 0) {
//        perror("ERROR opening socket");
//        exit(1);
//    }
//
//    server = gethostbyname(argv[1]);
//
//    if (server == NULL) {
//        fprintf(stderr, "ERROR, no such host\n");
//        exit(0);
//    }
//
//    bzero((char*) &serv_addr, sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//    bcopy(server->h_addr, (char*) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
//    serv_addr.sin_port = htons(portno);
//
//    /* Now connect to the server */
//    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
//        perror("ERROR connecting");
//        exit(1);
//    }
//    while (true) {
//        int type;
//        std::string text;
//        std::cin >> type;
//        std::cin >> text;
//
//        /* Send message to the server */
//        bool b = Message((Message::Type) type, text).Write(sockfd);
//
//        if (!b) {
//            perror("ERROR writing to socket");
//            exit(1);
//        }
//
//        /* Now Read server response */
//        Message m = Message::Read(sockfd);
//
//        if (m.type == Message::UNDEFINED) {
//            perror("ERROR reading from socket");
//            exit(1);
//        }
//
//
//        printf("Response: {type = %zu, length = %zu, message = %s}\n", m.type, m.body.size(),
//               m.body.c_str());
//
//    }
//    close(sockfd);
//    return 0;
//}

bool Message::Write(int sockfd) {
    size_t message_len = 2 * sizeof(uint32_t) + body.size();
    char* data = new char[message_len];
    PutInt32(type, data);
    PutInt32(static_cast<uint32_t>(body.size()), data + sizeof(uint32_t));
    PutBody(data + 2 * sizeof(uint32_t));

    for (ssize_t written = 0; message_len; written = ::write(sockfd, data, message_len)) {
        if (written < 0) {
            std::cerr << strerror(errno) << std::endl;
            return false;
        } else {
            data += written;
            message_len -= written;
        }
    }

    return true;
}

Message Message::Read(int sockfd) {
    uint32_t type, length;
    std::string data;
    if (GetInt32(&type, sockfd) &&
        GetInt32(&length, sockfd) &&
        GetBody(&data, length, sockfd)) {
        return Message(Type(type), data);
    }
    return Message();
}

void Message::PutInt32(uint32_t i, char* buf) {
    i = htonl(i);
    memcpy(buf, &i, sizeof(uint32_t));
}

void Message::PutBody(char* buf) {
    memcpy(buf, body.c_str(), body.size());
}

bool Message::GetInt32(uint32_t* i, int sockfd) {
    if (Get(reinterpret_cast<char*>(i), sizeof(uint32_t), sockfd)) {
        *i = ntohl(*i);
        return true;
    }
    return false;
}

bool Message::GetBody(std::string* body, size_t length, int sockfd) {
    char* buf = new char[length];
    if (Get(buf, length, sockfd)) {
        *body = std::string(buf);
        return true;
    }
    return false;
}

bool Message::Get(char* dst, size_t message_len, int sockfd) {
    for (ssize_t read = 0; message_len; read = ::read(sockfd, dst, message_len)) {
        if (read < 0) {
            std::cerr << strerror(errno) << std::endl;
            return false;
        } else {
            dst += read;
            message_len -= read;
        }
    }
    return true;
}
