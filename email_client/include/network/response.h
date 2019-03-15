#pragma once

#include "pch.h"
#include "email.h"

namespace response {

    enum ResponseStatus { OK, ERROR };

    class Response {
    public:
        ResponseStatus get_status() const;
        bool is_error();
    protected:
        explicit Response(ResponseStatus status);

    private:
        ResponseStatus status_;
    };

    class BadResponse : public Response {
    public:
        explicit BadResponse(const std::string &error_message);

        const std::string &get_error_message() const;

    private:
        std::string error_message_;
    };

    class SendResponse : public Response {
    public:
        explicit SendResponse();
    private:
    };

    class CheckResponse : public Response {
    public:
        explicit CheckResponse(const std::vector<email::EmailInfo>& infos);

        const std::vector<email::EmailInfo> &get_infos() const;

    private:
        std::vector<email::EmailInfo> infos_;
    };

    class GetResponse : public Response {
    public:
        explicit GetResponse(email::Email email);

        const email::Email &get_email() const;

    private:
        email::Email email_;
    };

} // namespace network