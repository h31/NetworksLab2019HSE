#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct dctp_response_header {
    uint8_t return_code;
    uint8_t operation_type;
    uint32_t id;
    int64_t result;
};

#pragma pack(pop)

enum return_code {
    OK = 0,
    WAIT_FOR_RESULT,
    OVERFLOW,
    DIV_BY_ZERO,
    FACT_OF_NEGATIVE,
    SQRT_OF_NEGATIVE,
    UNKNOWN_OPERATION
};

enum operation_type {
    FAST = 1,
    SLOW
};

inline std::string response_to_string(struct dctp_response_header response) {
    return "Return code: " + std::to_string(response.return_code)
           + "; type: " + std::to_string(response.operation_type)
           + "; id:  " + std::to_string(response.id)
           + "; result:  " + std::to_string(response.result);
}
