#include "../include/message.h"

RequestMessage RequestMessage::DISCONNECT() {
    return RequestMessage(RequestType::DISCONNECT);
}

RequestMessage RequestMessage::CONNECT() {
    return RequestMessage(RequestType::CONNECT);
}