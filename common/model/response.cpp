#include "response.h"

using namespace response;

Response::Response(ResponseBody *responseBody) : responseBody(responseBody) {
    status = dynamic_cast<ErrorResponseBody*>(responseBody) != nullptr ? OK : ERROR;
}

Status Response::getStatus() const {
    return status;
}

const ResponseBody &Response::getResponseBody() const {
    return *responseBody;
}

bool Response::isError() {
    return status == ERROR;
}

void EmptyResponseBody::accept(ResponseBodyVisitor *visitor) const  {
    visitor->visitEmptyResponseBody(this);
}

ErrorResponseBody::ErrorResponseBody(const std::string &message) : message(message) {}

const std::string &ErrorResponseBody::getMessage() const {
    return message;
}

void ErrorResponseBody::accept(ResponseBodyVisitor *visitor) const  {
    visitor->visitErrorResponseBody(this);
}

EmailInfosResponseBody::EmailInfosResponseBody(const std::vector<model::EmailInfo> &infos) : infos(infos) {}

const std::vector<model::EmailInfo> &EmailInfosResponseBody::getInfos() const {
    return infos;
}

void EmailInfosResponseBody::accept(ResponseBodyVisitor *visitor) const  {
    visitor->visitEmailInfosResponseBody(this);
}

EmailResponseBody::EmailResponseBody(const model::Email &email) : email(email) {}

const model::Email &EmailResponseBody::getEmail() const {
    return email;
}

void EmailResponseBody::accept(ResponseBodyVisitor *visitor) const  {
    visitor->visitEmailResponseBody(this);
}
