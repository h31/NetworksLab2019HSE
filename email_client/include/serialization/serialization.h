#pragma once

#include "pch.h"
#include "email.h"
#include "network/response.h"
#include "network/request.h"

namespace serialization {

    class SerializedMessage {
    public:
        SerializedMessage(size_t size_, const std::shared_ptr<uint8_t[]> &message_);

        size_t size() const;

        const uint8_t *get_message() const;

    private:
        size_t size_;
        std::shared_ptr<uint8_t[]> message_;
    };

    class Serializer {
    public:
        Serializer() = default;

        explicit Serializer(const request::Request *request);

        SerializedMessage serialize();

    private:

        const request::Request *request_;
        std::shared_ptr<uint8_t[]> buffer_;
        uint8_t *buf_;
        size_t index_;

        void write(uint32_t number);

        void write(const std::string &string);

        void write(request::RequestType type);

        void write(const email::Email &email);
    };

    class Deserializer {
    public:
        explicit Deserializer(const uint8_t *buffer, uint32_t offset = 0);

        uint32_t parse_uint32();

        std::string parse_string();

        response::ResponseStatus parse_response_status();

        email::Email parse_email();

        email::EmailInfo parse_email_info();

        std::vector<email::EmailInfo> parse_email_infos();

        std::shared_ptr<response::Response> parseResponse(request::RequestType type);

    private:
        const uint8_t *buffer_;
        size_t index_;
    };

} // namespace serialization