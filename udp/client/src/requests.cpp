#include "requests.hpp"

std::string errorCodeToString(uint8_t code) {
    switch (code) {
        case OK:
            return "OK";
        case OVERFLOW:
            return "Integer overflow";
        case DIV_BY_ZERO:
            return "Zero division error";
        case FACT_OF_NEGATIVE:
            return "Factorial of negative number";
        case SQRT_OF_NEGATIVE:
            return "Sqrt of negative number";
        default:
            return "Unknown error";
    }
}