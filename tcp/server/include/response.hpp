#pragma once

struct dctp_response_header {
    uint8_t return_code;
    uint32_t id;
    int64_t result;
};

enum return_code {
    OK = 0,
    OVERFLOW,
    DIV_BY_ZERO,
    FACT_OF_NEGATIVE,
    SQRT_OF_NEGATIVE
};