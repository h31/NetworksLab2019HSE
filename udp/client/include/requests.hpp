#pragma once

#include <cstdint>
#include <string>

#pragma pack(push, 1)
struct CalculatorResponse {
  uint8_t errorCode;
  uint8_t type;
  uint32_t computationId;
  int64_t result;
};
#pragma pack(pop)

enum OperationType {
  FAST = 1,
  SLOW
};

enum ErrorCode {
  OK = 0,
  WAIT_FOR_RESULT,
  OVERFLOW,
  DIV_BY_ZERO,
  FACT_OF_NEGATIVE,
  SQRT_OF_NEGATIVE,
  UNKNOWN_OP
};

std::string errorCodeToString(uint8_t code);

#pragma pack(push, 1)
struct CalculatorRequest {
  uint8_t type;
  uint32_t computationId;
  int64_t firstOperand;
  int64_t secondOperand;
};
#pragma pack(pop)

enum RequestType {
  PLUS = 1,
  MINUS,
  MULT,
  DIV,
  SQRT,
  FACT,
  LONG_OP_RESULT
};
