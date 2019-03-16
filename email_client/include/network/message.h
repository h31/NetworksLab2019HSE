#pragma once

#include "pch.h"

namespace network {

    class Message {
    public:
        virtual uint32_t size() const = 0;

        virtual ~Message() = default;

    };

} // namespace network