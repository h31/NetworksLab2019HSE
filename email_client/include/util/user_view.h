#pragma once

#include "pch.h"
#include "email.h"

namespace util {

    struct UserView {
        static std::string get_user_input(const std::string &str = "");

        static void println(const std::string &str = "");

        static void println(const email::Email &e);

        static void println(const email::EmailInfo &i);
    };


} // namespace util