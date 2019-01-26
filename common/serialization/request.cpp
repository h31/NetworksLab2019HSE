#include "request.h"
#include "size_collector.h"

using namespace serialization;

RequestSerializer::RequestSerializer(const request::Request *request)
    : request(request), messageSize(getSize(*request) + 4), buffer(new uint8_t[messageSize]), serializer(buffer.get()) {}

std::shared_ptr<uint8_t[]> RequestSerializer::serialize() {
    serializer.write(messageSize);
    serializer.write(request->getRequestType());
    request->accept(this);
    return buffer;
}

void RequestSerializer::visitSendEmailRequest(const request::SendEmailRequest *request) {
    serializer.write(request->getEmail());
}

void RequestSerializer::visitCheckEmailRequest(const request::CheckEmailRequest *request) {}

void RequestSerializer::visitGetEmailRequest(const request::GetEmailRequest *request) {
    serializer.write(request->getId());
}


RequestDeserializer::RequestDeserializer(const uint8_t *buffer) : deserializer(buffer) {}

std::shared_ptr<request::Request> RequestDeserializer::parseRequest() {
    request::RequestType type = deserializer.parseRequestType();
    switch (type) {
        case request::SEND_EMAIL:
            return parseSendEmailRequest();
        case request::CHECK_EMAIL:
            return parseCheckEmailRequest();
        case request::GET_EMAIL:
            return parseGetEmailRequest();
    }
}

std::shared_ptr<request::SendEmailRequest> RequestDeserializer::parseSendEmailRequest() {
    model::Email email = deserializer.parseEmail();
    return std::make_shared<request::SendEmailRequest>(email);
}

std::shared_ptr<request::CheckEmailRequest> RequestDeserializer::parseCheckEmailRequest() {
    return std::make_shared<request::CheckEmailRequest>();
}

std::shared_ptr<request::GetEmailRequest> RequestDeserializer::parseGetEmailRequest() {
    uint32_t id = deserializer.parseUInt32();
    return std::make_shared<request::GetEmailRequest>(id);
}
