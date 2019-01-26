#ifndef COMMON_SERIALIZATION_RESPONSE_H
#define COMMON_SERIALIZATION_RESPONSE_H

#include "primitive.h"

namespace serialization {
    class ResponseSerializer : response::ResponseBodyVisitor {
    private:
        const response::Response *response;
        uint32_t messageSize;
        std::shared_ptr<uint8_t[]> buffer;
        DataSerializer serializer;
    public:
        explicit ResponseSerializer(const response::Response *response);

        std::shared_ptr<uint8_t[]> serialize();

    private:
        void visitEmptyResponseBody(const response::EmptyResponseBody *responseBody) override;

        void visitErrorResponseBody(const response::ErrorResponseBody *responseBody) override;

        void visitEmailInfosResponseBody(const response::EmailInfosResponseBody *responseBody) override;

        void visitEmailResponseBody(const response::EmailResponseBody *responseBody) override;
    };

    class ResponseDeserializer {
    private:
        DataDeserializer deserializer;

    public:
        explicit ResponseDeserializer(const uint8_t *buffer);

        std::shared_ptr<response::Response> parseSendEmailResponse();
        std::shared_ptr<response::Response> parseCheckEmailResponse();
        std::shared_ptr<response::Response> parseGetEmailResponse();
    private:
        std::shared_ptr<response::Response> parseResponse(request::RequestType requestType);

        std::unique_ptr<response::EmptyResponseBody> parseEmptyResponseBody();
        std::unique_ptr<response::ErrorResponseBody> parseErrorResponseBody();
        std::unique_ptr<response::EmailInfosResponseBody> parseEmailInfosResponseBody();
        std::unique_ptr<response::EmailResponseBody> parseEmailResponseBody();
    };
}

#endif //COMMON_SERIALIZATION_RESPONSE_H
