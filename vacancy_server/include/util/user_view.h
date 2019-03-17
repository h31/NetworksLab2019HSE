#pragma once

#include "pch.h"

namespace util {

    struct UserView {
        static std::string get_user_input(const std::string &str = "");

        static void println(const std::string &str = "");
    };

} // namespace util