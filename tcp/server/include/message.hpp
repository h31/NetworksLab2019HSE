#pragma once

#include <cstdint>

#pragma pack(0)
struct pstp_request_header {
#define TEXT_UNIT_SIZE 64
    uint8_t type = 0;
    char wallet_id[TEXT_UNIT_SIZE] = "";
    char password[TEXT_UNIT_SIZE] = "";
    uint32_t content_size = 0;
};

#pragma pack(0)
struct pstp_response_header {
    uint8_t type = 0;
    uint8_t code = 0;
    uint32_t content_size = 0;
};

#pragma pack(0)
struct pstp_check_login_request {
    pstp_request_header header = {0, "", "", 0};
};
