#pragma once

#include "pch.h"

namespace network {

    struct socket_exception : std::exception {
        explicit socket_exception(std::string msg = "");

        const char *what() const noexcept override;

    private:
        std::string msg_;
    };

} // namespace network