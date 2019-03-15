#pragma once

#include "pch.h"

namespace network {


    struct Socket {

        explicit Socket(int descriptor);

        Socket(Socket && other) noexcept;

        Socket(const Socket & other) = default;

        virtual ~Socket();

//        Message read();

//        void write(const Message& message);

        void close();

        int descriptor() const;

    protected:
        int descriptor_;
        bool is_alive_;
    };
}