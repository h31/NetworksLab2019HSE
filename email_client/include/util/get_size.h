#pragma once

#include "pch.h"
#include "email.h"
//#include "email_system/request.h"
//#include "email_system/response.h"

namespace util {
    uint32_t get_size(const std::string &string);
    uint32_t get_size(const email::Email &email);
    uint32_t get_size(const email::EmailInfo &info);
//    uint32_t get_size(const std::vector<email::EmailInfo> &infos);
//    uint32_t getSize(const request::Request &request);
//    uint32_t getSize(const response::Response &response);
}