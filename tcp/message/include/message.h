#ifndef TCP_MESSAGE_H
#define TCP_MESSAGE_H

#include <string>

enum RequestType {
    CONNECT,
    MESSAGE,
    DISCONNECT
};

class RequestMessage {
    public:
        explicit RequestMessage(RequestType request_type): type_(request_type) {}

        RequestMessage(int32_t receiver, const std::string& message) : body_(message), type_(RequestType::MESSAGE) {}
        
        static RequestMessage DISCONNECT();

        static RequestMessage CONNECT();
        
    private:
        RequestType type_;

        std::string body_;

        int32_t receiver_id_;
};

#endif  // TCP_MESSAGE_H