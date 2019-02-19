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
        CONNECT       = 0x00,
        NEW_RATING    = 0x01,
        DELETE_RATING = 0x02,
        OPEN_RATING   = 0x03,
        CLOSE_RATING  = 0x04,
        ADD_OPTION    = 0x05,
        LIST_RATINGS  = 0x06,
        GET_RATING    = 0x07,
        VOTE          = 0x08
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
