#ifndef COMMON_SERIALIZATION_SIZE_COLLECTOR_H
#define COMMON_SERIALIZATION_SIZE_COLLECTOR_H

#include <model/request.h>
#include <model/response.h>

namespace serialization {
    uint32_t getSize(const std::string &string);
    uint32_t getSize(const model::Email &email);
    uint32_t getSize(const model::EmailInfo &info);
    uint32_t getSize(const std::vector<model::EmailInfo> &infos);
    uint32_t getSize(const request::Request &request);
    uint32_t getSize(const response::Response &response);


    class RequestSizeCollector : request::RequestVisitor {
    private:
        const request::Request *request;
        uint32_t size;

    public:
        explicit RequestSizeCollector(const request::Request *request);
        uint32_t getSize();

    private:
        void visitSendEmailRequest(const request::SendEmailRequest *request) override;

        void visitCheckEmailRequest(const request::CheckEmailRequest *request) override;

        void visitGetEmailRequest(const request::GetEmailRequest *request) override;
    };

    class ResponseBodySizeCollector : response::ResponseBodyVisitor {
    private:
        const response::ResponseBody *responseBody;
        uint32_t size;

    public:
        explicit ResponseBodySizeCollector(const response::ResponseBody *responseBody);
        uint32_t getSize();

    private:
        void visitEmptyResponseBody(const response::EmptyResponseBody *responseBody) override;

        void visitErrorResponseBody(const response::ErrorResponseBody *responseBody) override;

        void visitEmailInfosResponseBody(const response::EmailInfosResponseBody *responseBody) override;

        void visitEmailResponseBody(const response::EmailResponseBody *responseBody) override;
    };
}

#endif //COMMON_SERIALIZATION_SIZE_COLLECTOR_H
