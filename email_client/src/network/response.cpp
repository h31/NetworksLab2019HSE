#include <network/response.h>

namespace response {

    Response::Response(ResponseStatus status) : status_(status) {}

    bool Response::is_error() {
        return status_ == ERROR;
    }

    BadResponse::BadResponse(const std::string &error_message) : Response(ERROR), error_message_(error_message) {}

    const std::string &BadResponse::get_error_message() const {
        return error_message_;
    }

    SendResponse::SendResponse() : Response(OK) {}

    CheckResponse::CheckResponse(const std::vector<email::EmailInfo>& infos) : Response(OK), infos_(infos) {}

    const std::vector<email::EmailInfo> &CheckResponse::get_infos() const {
        return infos_;
    }

    GetResponse::GetResponse(email::Email email) : Response(OK), email_(std::move(email)) {}

    const email::Email &GetResponse::get_email() const {
        return email_;
    }

} // namespace response