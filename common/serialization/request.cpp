#include "request.h"
#include "size_collector.h"

using namespace serialization;

RequestSerializer::RequestSerializer(const request::Request *request)
    : request(request), messageSize(getSize(*request) + 4), buffer(new uint8_t[messageSize]), serializer(buffer.get()) {}

std::shared_ptr<uint8_t[]> RequestSerializer::serialize() {
    serializer.write(messageSize);
    serializer.write(request->getRequestType());
    serializer.write(request->getAuthor());
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
    std::string author = deserializer.parseString();
    switch (type) {
        case request::SEND_EMAIL:
            return parseSendEmailRequest(author);
        case request::CHECK_EMAIL:
            return parseCheckEmailRequest(author);
        case request::GET_EMAIL:
            return parseGetEmailRequest(author);
    }
}

std::shared_ptr<request::SendEmailRequest> RequestDeserializer::parseSendEmailRequest(const std::string &author) {
    model::Email email = deserializer.parseEmail();
    return std::make_shared<request::SendEmailRequest>(author, email);
}

std::shared_ptr<request::CheckEmailRequest> RequestDeserializer::parseCheckEmailRequest(const std::string &author) {
    return std::make_shared<request::CheckEmailRequest>(author);
}

std::shared_ptr<request::GetEmailRequest> RequestDeserializer::parseGetEmailRequest(const std::string &author) {
    uint32_t id = deserializer.parseUInt32();
    return std::make_shared<request::GetEmailRequest>(author, id);
}
