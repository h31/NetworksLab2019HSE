#include "./utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ios>

bool WriteString(int sockfd, std::string* str) {
    size_t len = str -> length();
    int n = write(sockfd, (char*) &len, sizeof(len));
    if (n <= 0)
        return false;
    
    n = write(sockfd, (char*) str -> c_str(), len);
    if (n <= 0)
        return false;
    return true;
}

std::string ReadString(int sockfd) {
    int len;
    int n = read(sockfd, &len, sizeof(size_t));
    if (n <= 0) 
        throw std::ios_base::failure("Error reading string");
}

bool WriteRequestMessage(int sockfd, const RequestMessage& message) {
    RequestType type = message.GetType();
    int n = write(sockfd, (char*) &type, sizeof(RequestType));
    if (n <= 0)
        return false;
    int32_t receiver_id = message.GetReceiverId();
    n = write(sockfd, (char*) &receiver_id, sizeof(receiver_id));
    if (n <= 0)
        return false;
    if (!WriteString(sockfd, message.GetBody()))
        return false;
    return true;
}

RequestMessage ReadRequestMessage(int sockfd) {
    int32_t receiver_id;
    RequestType type;
    int n = read(sockfd, (char*) &type, sizeof(type));
    if (n <= 0)
        throw std::ios_base::failure("Error reading request message");
    n = read(sockfd, (char*) &receiver_id, sizeof(receiver_id));
    if (n <= 0)
        throw std::ios_base::failure("Error reading request message");
    std::string body = ReadString(sockfd);
    if (type == RequestType::CONNECT)
        return RequestMessage::CONNECT();
    else if (type == RequestType::DISCONNECT)
        return RequestMessage::DISCONNECT();
    else 
        return RequestMessage(receiver_id, body);
}

bool WriteResponseMessage(int sockfd, const ResponseMessage& message) {
    return false;  // TODO
}

ResponseMessage ReadResponseMessage(int sockfd) {
    return ResponseMessage(ResponseType::MESSAGE);  // TODO
}
