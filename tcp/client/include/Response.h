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
        RATING_STATS  = 0x04,
        /* Auxiliary state meaning end of user input */
        EXIT          = 0xFF
    };

    ResponseType type;
    std::vector<RequestField> fields;

    explicit Response(ResponseType type);
    Response &readField(SocketReader &reader, RequestField::Type rtype);

    bool isDisconnect();
    bool isExit();
    std::string getError();

public:
    static Response readResponse(SocketReader &reader);
    static Response ResponseDisconnect();
    bool isError();
    bool checkExit();
    void print();
    static Response exitResponse();
};


#endif
