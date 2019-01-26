#include "size_collector.h"

using namespace serialization;

uint32_t serialization::getSize(const std::string &string) {
    return static_cast<uint32_t>(4 + string.size());
}
uint32_t serialization::getSize(const model::Email &email) {
    uint32_t result = getSize(email.getTheme());
    result += getSize(email.getBody());
    result += getSize(email.getAuthor());
    result += getSize(email.getRecipient());
    return result;
}

uint32_t serialization::getSize(const model::EmailInfo &info) {
    uint32_t result = 4;
    result += getSize(info.getAuthor());
    result += getSize(info.getTheme());
    return result;
}

uint32_t serialization::getSize(const std::vector<model::EmailInfo> &infos) {
    uint32_t result = 4;
    for (auto & info : infos) {
        result += getSize(info);
    }
    return result;
}

uint32_t serialization::getSize(const request::Request &request) {
    return RequestSizeCollector(&request).getSize();
}

uint32_t serialization::getSize(const response::Response &response) {
    return ResponseBodySizeCollector(&response.getResponseBody()).getSize() + 1; // + status
}

RequestSizeCollector::RequestSizeCollector(const request::Request *request) : request(request), size(0) {}

uint32_t RequestSizeCollector::getSize() {
    if (size == 0) {
        size += 1; // type
        request->accept(this);
    }
    return size;
}

void RequestSizeCollector::visitSendEmailRequest(const request::SendEmailRequest *request) {
    size += ::getSize(request->getEmail());
}

void RequestSizeCollector::visitCheckEmailRequest(const request::CheckEmailRequest *request) {}

void RequestSizeCollector::visitGetEmailRequest(const request::GetEmailRequest *request) {
    size += 4;
}

ResponseBodySizeCollector::ResponseBodySizeCollector(const response::ResponseBody *responseBody) : responseBody(responseBody), size(0) {}

uint32_t ResponseBodySizeCollector::getSize() {
    if (size == 0) {
        responseBody->accept(this);
    }
    return size;
}

void ResponseBodySizeCollector::visitEmptyResponseBody(const response::EmptyResponseBody *responseBody) {}

void ResponseBodySizeCollector::visitErrorResponseBody(const response::ErrorResponseBody *responseBody) {
    size += ::getSize(responseBody->getMessage());
}

void ResponseBodySizeCollector::visitEmailInfosResponseBody(const response::EmailInfosResponseBody *responseBody) {
    size += ::getSize(responseBody->getInfos());
}

void ResponseBodySizeCollector::visitEmailResponseBody(const response::EmailResponseBody *responseBody) {
    size += ::getSize(responseBody->getEmail());
}
