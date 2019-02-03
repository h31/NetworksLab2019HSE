#include <model/response.h>

using namespace response;

Response::Response(std::shared_ptr<ResponseBody> responseBody) : responseBody(responseBody) {
    status = dynamic_cast<ErrorResponseBody*>(responseBody.get()) == nullptr ? OK : ERROR;
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

bool Response::operator==(const Response &other) const {
    return (status == other.status) && (*responseBody == *other.responseBody);
}

void EmptyResponseBody::accept(ResponseBodyVisitor *visitor) const  {
    visitor->visitEmptyResponseBody(this);
}

bool EmptyResponseBody::operator==(const ResponseBody &other) const {
    const auto *pOther = dynamic_cast<const EmptyResponseBody*>(&other);
    return pOther != nullptr;
}

ErrorResponseBody::ErrorResponseBody(const std::string &message) : message(message) {}

const std::string &ErrorResponseBody::getMessage() const {
    return message;
}

void ErrorResponseBody::accept(ResponseBodyVisitor *visitor) const  {
    visitor->visitErrorResponseBody(this);
}
bool ErrorResponseBody::operator== (const ResponseBody &other) const {
    const auto *pOther = dynamic_cast<const ErrorResponseBody*>(&other);
    if (pOther == nullptr) return false;
    return message == pOther->message;
}

EmailInfosResponseBody::EmailInfosResponseBody(const std::vector<model::EmailInfo> &infos) : infos(infos) {}

const std::vector<model::EmailInfo> &EmailInfosResponseBody::getInfos() const {
    return infos;
}

void EmailInfosResponseBody::accept(ResponseBodyVisitor *visitor) const  {
    visitor->visitEmailInfosResponseBody(this);
}

bool EmailInfosResponseBody::operator== (const ResponseBody &other) const {
    const auto *pOther = dynamic_cast<const EmailInfosResponseBody*>(&other);
    if (pOther == nullptr) return false;
    return infos == pOther->infos;
}

EmailResponseBody::EmailResponseBody(const model::Email &email) : email(email) {}

const model::Email &EmailResponseBody::getEmail() const {
    return email;
}

void EmailResponseBody::accept(ResponseBodyVisitor *visitor) const  {
    visitor->visitEmailResponseBody(this);
}

bool EmailResponseBody::operator== (const ResponseBody &other) const {
    const auto *pOther = dynamic_cast<const EmailResponseBody*>(&other);
    if (pOther == nullptr) return false;
    return email == pOther->email;
}
