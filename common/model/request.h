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

    protected:
        explicit Request(RequestType _requestType);

    public:
        RequestType getRequestType() const;

        virtual void accept(RequestVisitor *visitor) const = 0;

        ~Request() = default;
    };

    class SendEmailRequest : public Request {
    private:
        model::Email email;
    public:
        explicit SendEmailRequest(const model::Email &email);

        const model::Email &getEmail() const;

        void accept(RequestVisitor *visitor) const override;
    };

    class CheckEmailRequest : public Request {
    public:
        CheckEmailRequest();

        void accept(RequestVisitor *visitor) const override;
    };

    class GetEmailRequest : public Request {
    private:
        uint32_t id;
    public:
        explicit GetEmailRequest(uint32_t id);

        uint32_t getId() const;

        void accept(RequestVisitor *visitor) const override;
    };

    class RequestVisitor {
    public:
        virtual void visitSendEmailRequest(const SendEmailRequest *request) = 0;
        virtual void visitCheckEmailRequest(const CheckEmailRequest *request) = 0;
        virtual void visitGetEmailRequest(const GetEmailRequest *request) = 0;
    };
}



#endif //COMMON_MODEL_REQUEST_H
