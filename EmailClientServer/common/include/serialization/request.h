#ifndef COMMON_SERIALIZATION_REQUEST_H
#define COMMON_SERIALIZATION_REQUEST_H

#include <model/request.h>
#include <communication/communication.h>
#include "primitive.h"

namespace serialization {

    class RequestSerializer : request::RequestVisitor {
    private:
        const request::Request *request;
        uint32_t messageSize;
        std::shared_ptr<uint8_t[]> buffer;
        DataSerializer serializer;

    public:
        explicit RequestSerializer(const request::Request *request);

        communication::encoded_message serialize();

    private:
        void visitSendEmailRequest(const request::SendEmailRequest *request) override;

        void visitCheckEmailRequest(const request::CheckEmailRequest *request) override;

        void visitGetEmailRequest(const request::GetEmailRequest *request) override;
    };


    class RequestDeserializer {
    private:
        DataDeserializer deserializer;

    public:
        explicit RequestDeserializer(const uint8_t *buffer, uint32_t offset = 0);

        std::shared_ptr<request::Request> parseRequest();

    private:
        std::shared_ptr<request::SendEmailRequest> parseSendEmailRequest(const std::string &author);
        std::shared_ptr<request::CheckEmailRequest> parseCheckEmailRequest(const std::string &author);
        std::shared_ptr<request::GetEmailRequest> parseGetEmailRequest(const std::string &author);
    };
}

#endif //COMMON_SERIALIZATION_REQUEST_H