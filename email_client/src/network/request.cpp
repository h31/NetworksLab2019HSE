#include <network/request.h>

namespace request {
    using namespace util;

    Request::Request(RequestType type, const std::string &author) : type_(type), author_(author) {}

    RequestType Request::get_type() const {
        return type_;
    }

    const std::string &Request::get_author() const {
        return author_;
    }

    uint32_t Request::size() const {
        return TYPE_SIZE + get_size(get_author());
    }

    SendRequest::SendRequest(email::Email email) : Request(SEND_EMAIL, email.get_author()), email_(std::move(email)) {}

    const email::Email &SendRequest::get_email() const {
        return email_;
    }

    RequestType SendRequest::get_type() const {
        return Request::get_type();
    }

    uint32_t SendRequest::size() const {
        return Request::size() + get_size(email_);
    }

    CheckRequest::CheckRequest(const std::string &author) : Request(CHECK_EMAIL, author) {}

    uint32_t CheckRequest::size() const {
        return Request::size();
    }

    GetRequest::GetRequest(const std::string &author, uint32_t id) : Request(GET_EMAIL, author), id_(id), size_(0) {}

    uint32_t GetRequest::get_id() const {
        return id_;
    }

    uint32_t GetRequest::size() const {
        return Request::size() + INT_SIZE;
    }
} // namespace request