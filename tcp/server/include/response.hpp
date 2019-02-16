#pragma once

#include <cstdint>

struct dctp_response_header {
    uint8_t return_code;
    uint8_t operation_type;
    uint32_t id;
    int64_t result;
};

enum return_code {
    OK = 0,
    WAIT_FOR_RESULT,
    OVERFLOW,
    DIV_BY_ZERO,
    FACT_OF_NEGATIVE,
    SQRT_OF_NEGATIVE,
};

enum operation_type {
    FAST = 1,
    SLOW
};