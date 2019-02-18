#include "./message.h"

RequestMessage RequestMessage::DISCONNECT() {
    return RequestMessage(RequestType::DISCONNECT);
}

RequestMessage RequestMessage::CONNECT() {
    return RequestMessage(RequestType::CONNECT);
}

RequestType RequestMessage::GetType() const {
    return type_;
}

std::string RequestMessage::GetBody() const {
    return body_;
}

int32_t RequestMessage::GetReceiverId() const {
    return receiver_id_;
}



ResponseMessage ResponseMessage::CONNECT_FAIL() {
    return ResponseMessage(ResponseType::CONNECT_FAIL);
}

ResponseMessage ResponseMessage::CONNECT_OK() {
    return ResponseMessage(ResponseType::CONNECT_OK);
}

ResponseType ResponseMessage::GetType() const {
    return type_;
}

std::string ResponseMessage::GetBody() const {
    return body_;
}

int32_t ResponseMessage::GetSenderId() const {
    return sender_id_;
}
