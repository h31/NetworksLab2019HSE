#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <vector>
#include <unordered_map>
#include <functional>

#include "Response.h"
#include "request/RequestField.h"

class Client {

private:
    enum class RequestType : uint8_t {
        CONNECT       = 0x01,
        NEW_RATING    = 0x02,
        DELETE_RATING = 0x03,
        OPEN_RATING   = 0x04,
        CLOSE_RATING  = 0x05,
        ADD_OPTION    = 0x06,
        LIST_RATINGS  = 0x07,
        GET_RATING    = 0x08,
        VOTE          = 0x09
    };

    std::unordered_map<std::string, std::function<bool()>> commands;
    int socketfd;

    Response performRequest(RequestType type, std::vector<RequestField> fields);
    bool connect();
    bool newRating();
    bool deleteRating();
    bool openRating();
    bool closeRating();
    bool addOption();
    bool listRatings();
    bool showRating();
    bool vote();
    void help();

public:
    explicit Client(int socketfd);
    void run();
};


#endif
