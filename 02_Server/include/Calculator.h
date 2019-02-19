#pragma once

#include <stddef.h>
#include <cstdint>
#include <cmath>
#include <iostream>

class Calculator
{
public:
    Calculator() = delete;

    static double get_sum(double a, double b);
    static double get_diff(double a, double b);
    static double get_mul(double a, double b);
    static double get_quot(double a, double b);
    static uint64_t get_fact(uint64_t a);
    static double get_sqrt(double a);
};