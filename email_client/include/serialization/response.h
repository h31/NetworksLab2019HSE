#pragma once

#include "pch.h"
#include "network/response.h"

namespace serialization {


    class ResponseDeserializer {
    public:
        explicit ResponseDeserializer(const uint8_t *buffer, uint32_t offset = 0);

        std::shared_ptr<response::Response> parseResponse(request::RequestType type);

    private:

        Deserializer deserializer_;
    };

} // namespace serialization