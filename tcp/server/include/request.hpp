#pragma once

#include <cstdint>

#pragma pack(0)
struct dctp_request_header {
    uint8_t type;
    uint32_t id;
    int64_t first_operand;
    int64_t second_operand;
};

enum request_type {
    PLUS = 1,
    MINUS,
    MULT,
    DIV,
    SQRT,
    FACT
};