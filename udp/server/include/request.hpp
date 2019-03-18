#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct dctp_request_header {
    uint8_t type;
    uint32_t id;
    int64_t first_operand;
    int64_t second_operand;
};

#pragma pack(pop)

enum request_type {
    PLUS = 1,
    MINUS,
    MULT,
    DIV,
    SQRT,
    FACT,
    LONG_COMPUTATION_RESULT
};

std::string request_to_string(struct dctp_request_header request) {
    return "Id: " + std::to_string(request.id)
    + "; type: " + std::to_string(request.type)
    + "; first operand:  " + std::to_string(request.first_operand)
    + "; second operand:  " + std::to_string(request.second_operand);
}