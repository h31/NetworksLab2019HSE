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

        RequestMessage(int32_t receiver, const std::string& message) : body_(message), type_(RequestType::MESSAGE), receiver_id_(receiver) {}
        
        static RequestMessage DISCONNECT();

        static RequestMessage CONNECT();
        
        RequestType GetType() const;
        
        std::string GetBody() const;

        int32_t GetReceiverId() const;

    private:
        RequestType type_;

        std::string body_ = '';

        int32_t receiver_id_= 0;
};

#endif  // TCP_MESSAGE_H