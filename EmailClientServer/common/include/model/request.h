#ifndef COMMON_MODEL_REQUEST_H
#define COMMON_MODEL_REQUEST_H

#include <string>
#include "model.h"

namespace request {
    enum RequestType {
        SEND_EMAIL = 1,
        CHECK_EMAIL = 2,
        GET_EMAIL = 3
    };

    class RequestVisitor;

    class Request {
    private:
        RequestType requestType;
        const std::string author;

    protected:
        Request(RequestType requestType, std::string author);

    public:
        RequestType getRequestType() const;

        const std::string &getAuthor() const;

        virtual void accept(RequestVisitor *visitor) const = 0;

        ~Request() = default;
    };

    class SendEmailRequest : public Request {
    private:
        model::Email email;
    public:
        SendEmailRequest(std::string author, const model::Email &email);

        const model::Email &getEmail() const;

        void accept(RequestVisitor *visitor) const override;

        bool operator == (const SendEmailRequest &other) const;
    };

    class CheckEmailRequest : public Request {
    public:
        explicit CheckEmailRequest(std::string author);

        void accept(RequestVisitor *visitor) const override;

        bool operator == (const CheckEmailRequest &other) const;
    };

    class GetEmailRequest : public Request {
    private:
        uint32_t id;
    public:
        GetEmailRequest(std::string author, uint32_t id);

        uint32_t getId() const;

        void accept(RequestVisitor *visitor) const override;

        bool operator == (const GetEmailRequest &other) const;
    };

    class RequestVisitor {
    public:
        virtual void visitSendEmailRequest(const SendEmailRequest *request) = 0;
        virtual void visitCheckEmailRequest(const CheckEmailRequest *request) = 0;
        virtual void visitGetEmailRequest(const GetEmailRequest *request) = 0;
    };
}



#endif //COMMON_MODEL_REQUEST_H
