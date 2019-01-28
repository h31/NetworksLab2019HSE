#ifndef COMMON_MODEL_RESPONSE_H
#define COMMON_MODEL_RESPONSE_H

#include <memory>
#include <vector>
#include "model.h"

namespace response {
    enum Status {
        OK = 0,
        ERROR = 1
    };

    class ResponseBodyVisitor;

    class ResponseBody {
    protected:
        ResponseBody() = default;
    public:
        virtual void accept(ResponseBodyVisitor *visitor) const = 0;

        virtual ~ResponseBody() = default;
    };

    class EmptyResponseBody : public ResponseBody {
    public:
        EmptyResponseBody() = default;

        void accept(ResponseBodyVisitor *visitor) const  override;
    };

    class ErrorResponseBody : public ResponseBody {
    private:
        std::string message;
    public:
        explicit ErrorResponseBody(const std::string &message);

        const std::string &getMessage() const;

        void accept(ResponseBodyVisitor *visitor) const  override;
    };

    class EmailInfosResponseBody : public ResponseBody {
    private:
        std::vector<model::EmailInfo> infos;
    public:
        explicit EmailInfosResponseBody(const std::vector<model::EmailInfo> &infos);

        const std::vector<model::EmailInfo> &getInfos() const;

        void accept(ResponseBodyVisitor *visitor) const  override;
    };

    class EmailResponseBody : public ResponseBody {
    private:
        model::Email email;
    public:
        explicit EmailResponseBody(const model::Email &email);

        const model::Email &getEmail() const;

        void accept(ResponseBodyVisitor *visitor) const  override;
    };

    class Response {
    private:
        std::unique_ptr<ResponseBody> responseBody;
        Status status;
    public:
        explicit Response(ResponseBody *responseBody);

        ~Response() = default;

        Status getStatus() const;

        bool isError();

        const ResponseBody &getResponseBody() const;
    };

    class ResponseBodyVisitor {
    public:
        virtual void visitEmptyResponseBody(const EmptyResponseBody *responseBody) = 0;
        virtual void visitErrorResponseBody(const ErrorResponseBody *responseBody) = 0;
        virtual void visitEmailInfosResponseBody(const EmailInfosResponseBody *responseBody) = 0;
        virtual void visitEmailResponseBody(const EmailResponseBody *responseBody) = 0;
        virtual ~ResponseBodyVisitor() = default;
    };
};

#endif //COMMON_MODEL_RESPONSE_H
