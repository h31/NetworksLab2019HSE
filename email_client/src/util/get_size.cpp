#include "util/get_size.h"

namespace util {

    uint32_t get_size(const std::string &string) {
        return static_cast<uint32_t>(INT_SIZE + string.size());
    }

    uint32_t get_size(const email::Email &email) {
        uint32_t result = 0;
        result += get_size(email.get_recipient());
        result += get_size(email.get_author());
        result += get_size(email.get_theme());
        result += get_size(email.get_body());
        return result;
    }

    uint32_t get_size(const email::EmailInfo &info) {
        uint32_t result = INT_SIZE;
        result += get_size(info.get_author());
        result += get_size(info.get_theme());
        return result;
    }

    uint32_t get_size(const std::vector<email::EmailInfo> &infos) {
        uint32_t result = INT_SIZE;
        for (auto & info : infos) {
            result += get_size(info);
        }
        return result;
    }

}