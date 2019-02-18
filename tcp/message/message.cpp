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
