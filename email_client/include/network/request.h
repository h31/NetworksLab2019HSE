#pragma once

#include "pch.h"
#include "email.h"
#include "network/message.h"

namespace request {

    enum RequestType {
        SEND_EMAIL = 1,
        CHECK_EMAIL = 2,
        GET_EMAIL = 3
    };

    class Request : public network::Message {
    protected:
        Request(RequestType type, const std::string &author);

    public:
        uint32_t size() const override;

        virtual RequestType get_type() const;

        const std::string &get_author() const;

    private:
        RequestType type_;
        const std::string author_;
    };

    class SendRequest : public Request {
    public:
        explicit SendRequest(email::Email email);

        const email::Email &get_email() const;

        RequestType get_type() const override;

        uint32_t size() const override;

    private:
        email::Email email_;
    };

    class CheckRequest : public Request {
    public:
        explicit CheckRequest(const std::string &author);

        uint32_t size() const override;

    };

    class GetRequest : public Request {
    public:
        GetRequest(const std::string &author, uint32_t id);

        uint32_t get_id() const;

    protected:
        uint32_t size() const override;

    private:
        uint32_t id_;
        uint32_t size_;
    };

} // namespace request