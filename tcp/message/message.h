#ifndef TCP_MESSAGE_H
#define TCP_MESSAGE_H

#include <string>

enum RequestType {
    CONNECT,
    MESSAGE,
    DISCONNECT
};

enum ResponseType {
    CONNECT_OK,
    CONNECT_FAIL,
    MESSAGE
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

        std::string body_ = "";

        int32_t receiver_id_= 0;
};

class ResponseMessage {
    public:
        explicit ResponseMessage(ResponseType response_type): type_(response_type) {}
    
        ResponseMessage(int32_t sender, const std::string& message) : body_(message), type_(ResponseType::MESSAGE), sender_id_(sender) {}
    
        static ResponseMessage CONNECT_FAIL();
    
        static ResponseMessage CONNECT_OK();
    
        ResponseType GetType() const;
    
        std::string GetBody() const;
    
        int32_t GetSenderId() const;

    private:
        ResponseType type_;
        
        std::string body_ = "";
        
        int32_t sender_id_ = 0;
};

#endif  // TCP_MESSAGE_H