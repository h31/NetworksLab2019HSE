#include "serialization/response.h"
#include "serialization/size_collector.h"

using namespace serialization;

ResponseSerializer::ResponseSerializer(const response::Response *response)
    : response(response), messageSize(getSize(*response) + 4), buffer(new uint8_t[messageSize]), serializer(buffer.get()) {}

std::pair<uint32_t, std::shared_ptr<uint8_t[]>> ResponseSerializer::serialize() {
    serializer.write(messageSize);
    serializer.write(response->getStatus());
    response->getResponseBody().accept(this);
    return std::make_pair(messageSize, buffer);
}

void ResponseSerializer::visitEmptyResponseBody(const response::EmptyResponseBody *responseBody) {}

void ResponseSerializer::visitErrorResponseBody(const response::ErrorResponseBody *responseBody) {
    serializer.write(responseBody->getMessage());
}

void ResponseSerializer::visitEmailInfosResponseBody(const response::EmailInfosResponseBody *responseBody) {
    serializer.write(responseBody->getInfos());
}

void ResponseSerializer::visitEmailResponseBody(const response::EmailResponseBody *responseBody) {
    serializer.write(responseBody->getEmail());
}


ResponseDeserializer::ResponseDeserializer(const uint8_t *buffer) : deserializer(buffer) {}

std::shared_ptr<response::Response> ResponseDeserializer::parseSendEmailResponse() {
    return parseResponse(request::SEND_EMAIL);
}

std::shared_ptr<response::Response> ResponseDeserializer::parseCheckEmailResponse() {
    return parseResponse(request::CHECK_EMAIL);
}

std::shared_ptr<response::Response> ResponseDeserializer::parseGetEmailResponse() {
    return parseResponse(request::GET_EMAIL);
}

std::shared_ptr<response::Response> ResponseDeserializer::parseResponse(request::RequestType requestType) {
    response::Status status = deserializer.parseResponseStatus();
    std::unique_ptr<response::ResponseBody> responseBody;
    if (status == response::OK) {
        switch (requestType) {
            case request::SEND_EMAIL:
                responseBody = parseEmptyResponseBody();
                break;
            case request::CHECK_EMAIL:
                responseBody = parseEmailInfosResponseBody();
                break;
            case request::GET_EMAIL:
                responseBody = parseEmailResponseBody();
                break;
        }
    } else {
        responseBody = parseErrorResponseBody();
    }
    return std::make_shared<response::Response>(responseBody.get());
}

std::unique_ptr<response::EmptyResponseBody> ResponseDeserializer::parseEmptyResponseBody() {
    return std::make_unique<response::EmptyResponseBody>();
}

std::unique_ptr<response::ErrorResponseBody> ResponseDeserializer::parseErrorResponseBody() {
    std::string message = deserializer.parseString();
    return std::make_unique<response::ErrorResponseBody>(message);
}

std::unique_ptr<response::EmailInfosResponseBody> ResponseDeserializer::parseEmailInfosResponseBody() {
    std::vector<model::EmailInfo> infos = deserializer.parseEmailInfos();
    return std::make_unique<response::EmailInfosResponseBody>(infos);
}

std::unique_ptr<response::EmailResponseBody> ResponseDeserializer::parseEmailResponseBody() {
    model::Email email = deserializer.parseEmail();
    return std::make_unique<response::EmailResponseBody>(email);
}








