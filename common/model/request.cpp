#include "request.h"

using namespace request;

Request::Request(RequestType _requestType) : requestType(_requestType) {}

RequestType Request::getRequestType() const {
    return requestType;
}

SendEmailRequest::SendEmailRequest(const model::Email &email) : Request(SEND_EMAIL), email(email) {}

const model::Email &SendEmailRequest::getEmail() const {
    return email;
}

void SendEmailRequest::accept(RequestVisitor *visitor) const {
    visitor->visitSendEmailRequest(this);
}

CheckEmailRequest::CheckEmailRequest() : Request(CHECK_EMAIL) {}

void CheckEmailRequest::accept(RequestVisitor *visitor) const {
    visitor->visitCheckEmailRequest(this);
}

GetEmailRequest::GetEmailRequest(uint32_t id) : Request(GET_EMAIL), id(id) {}

uint32_t GetEmailRequest::getId() const {
    return id;
}

void GetEmailRequest::accept(RequestVisitor *visitor) const {
    visitor->visitGetEmailRequest(this);
}
