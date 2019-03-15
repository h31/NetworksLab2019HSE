#pragma once

#include "pch.h"
#include "email.h"
#include "network/response.h"
#include "network/request.h"

namespace serialization {

    using encoded_message = std::pair<uint32_t, std::shared_ptr<uint8_t[]>>;
    using decoded_message = std::shared_ptr<request::Request>;

    class Serializer {
    public:

        Serializer() = default;

        explicit Serializer(uint8_t *buffer);

        void write(uint32_t number);

        void write(const std::string &string);

        void write(request::RequestType type);

        void write(response::ResponseStatus status);

        void write(const email::Email &email);

        void write(const email::EmailInfo &info);

        void write(const std::vector<email::EmailInfo> &infos);

        encoded_message serialize(const request::SendRequest &request);

        encoded_message serialize(const request::GetRequest &request);

        encoded_message serialize(const request::CheckRequest &request);

    private:
        uint8_t *buffer_;
        size_t index_;
    };

    class Deserializer {
    public:
        explicit Deserializer(const uint8_t *buffer, uint32_t offset = 0);

        uint32_t parse_uint32();

        std::string parse_string();

        request::RequestType parse_request_type();

        response::ResponseStatus parse_response_status();

        email::Email parse_email();

        email::EmailInfo parse_email_info();

        std::vector<email::EmailInfo> parse_email_infos();

    private:
        const uint8_t *buffer_;
        size_t index_;
    };

} // namespace serialization