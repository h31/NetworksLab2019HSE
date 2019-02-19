#ifndef CLIENT_RESPONSE_H
#define CLIENT_RESPONSE_H


#include <string>
#include <vector>
#include <request/RequestField.h>

class Response {

private:
    enum class ResponseType : uint8_t {
        SUCCESS       = 0x00,
        ERROR         = 0x01,
        DISCONNECT    = 0x02,
        RATING_LIST   = 0x03,
        RATING_STATS  = 0x04
    };

    ResponseType type;
    std::vector<RequestField> fields;

    explicit Response(ResponseType type);
    Response &readField(int socketfd, RequestField::Type rtype);

public:
    static Response readResponse(int socketfd);
    static Response ResponseDisconnect();
    bool isError();
    bool isDisconnect();
    bool checkDisconnect();
    std::string getError();
    void print();
};


#endif
