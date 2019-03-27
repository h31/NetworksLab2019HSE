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

int readn( SOCKET fd, char *bp, size_t len)
{
    int cnt;
    int rc
    cnt = len;
    while ( cnt > 0 )
    {
        rc = recv( fd, bp, cnt, 0 ) ;
        if ( rc < 0 )
        {
            if ( errno == EINTR )
            continue;
            return -1;
        }
        if ( rc == 0 )
        return len - cnt; /* Вернуть неполный счетчик. */
        bp + = rc;
        cnt -= rc;
    }
    return len;
}

std::string ReadString(int sockfd) {
    int len;
    int n = read(sockfd, &len, sizeof(size_t));
    if (n <= 0)
        throw std::ios_base::failure("Error reading string");
    std::string result(len, ' ');
    n = readn(sockfd, &result[0], len);
    if (n <= 0)
        throw std::ios_base::failure("Error reading string");
    return result;
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
    std::string body = message.GetBody();
    if (!WriteString(sockfd, &body))
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
    ResponseType type = message.GetType();
    int n = write(sockfd, (char*) &type, sizeof(ResponseType));
    if (n <= 0)
        return false;
    int32_t sender_id = message.GetSenderId();
    n = write(sockfd, (char*) &sender_id, sizeof(sender_id));
    if (n <= 0)
        return false;
    std::string body = message.GetBody();
    if (!WriteString(sockfd, &body))
        return false;
    return true;
}

ResponseMessage ReadResponseMessage(int sockfd) {
    int32_t sender_id;
    ResponseType type;
    int n = read(sockfd, (char*) &type, sizeof(type));
    if (n <= 0)
        throw std::ios_base::failure("Error reading response message");
    n = read(sockfd, (char*) &sender_id, sizeof(sender_id));
    if (n <= 0)
        throw std::ios_base::failure("Error reading response message");
    std::string body = ReadString(sockfd);
    if (type == ResponseType::CONNECT_OK)
        return ResponseMessage::CONNECT_OK();
    else if (type == ResponseType::CONNECT_FAIL)
        return ResponseMessage::CONNECT_FAIL();
    else
        return ResponseMessage(sender_id, body);
}
