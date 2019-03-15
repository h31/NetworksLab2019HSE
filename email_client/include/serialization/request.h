#pragma once

#include "pch.h"
#include "network/request.h"
#include "serialization/serialization.h"

namespace serialization {

    class RequestSerializer {
    public:
        explicit RequestSerializer(const request::Request *request);

        encoded_message serialize();

    private:
        const request::Request *request_;
        std::shared_ptr<uint8_t[]> buffer_;
        Serializer serializer_;
    };

} // namespace serialization