#include <utility>
#include <model/request.h>

using namespace request;

Request::Request(RequestType requestType, std::string author) : requestType(requestType), author(std::move(author)) {}

RequestType Request::getRequestType() const {
    return requestType;
}

const std::string &Request::getAuthor() const {
    return author;
}

SendEmailRequest::SendEmailRequest(std::string author, const model::Email &email) : Request(SEND_EMAIL, std::move(author)), email(email) {}

const model::Email &SendEmailRequest::getEmail() const {
    return email;
}

void SendEmailRequest::accept(RequestVisitor *visitor) const {
    visitor->visitSendEmailRequest(this);
}

CheckEmailRequest::CheckEmailRequest(std::string author) : Request(CHECK_EMAIL, std::move(author)) {}

void CheckEmailRequest::accept(RequestVisitor *visitor) const {
    visitor->visitCheckEmailRequest(this);
}

GetEmailRequest::GetEmailRequest(std::string author, uint32_t id) : Request(GET_EMAIL, std::move(author)), id(id) {}

uint32_t GetEmailRequest::getId() const {
    return id;
}

void GetEmailRequest::accept(RequestVisitor *visitor) const {
    visitor->visitGetEmailRequest(this);
}
