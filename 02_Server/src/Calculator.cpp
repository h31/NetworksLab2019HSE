#include "Calculator.h"

double Calculator::get_sum(double a, double b)
{
    return a + b;
}

double Calculator::get_diff(double a, double b)
{
    return a - b;
}

double Calculator::get_mul(double a, double b)
{
    return a * b;
}

double Calculator::get_quot(double a, double b)
{
    return a / b;
}

uint64_t Calculator::get_fact(uint64_t a)
{
    uint64_t res = 1;
    for (uint64_t i = 1; i <= a; i++) {
        res *= i;
    }
    return res;
}

double Calculator::get_sqrt(double a)
{
    return sqrt(a);
}