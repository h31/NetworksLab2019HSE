#include <cstring>
#include <cstdio>
#include <memory>
#include "include/message.h"

std::unique_ptr<char> Calculation::Serialize() const {
    std::unique_ptr<char> result(new char[sizeof(char) + sizeof(int) + sizeof(int)]);
    memcpy(result.get(), &(this->operation), sizeof(char));
    memcpy(result.get() + sizeof(char), &(this->arg_left), sizeof(int));
    memcpy(result.get() + sizeof(char) + sizeof(int), &(this->arg_right), sizeof(int));
    memcpy(result.get() + sizeof(char) + 2 * sizeof(int), &(this->result), sizeof(double));
    return result;
}

Calculation Calculation::Deserialize(char* bytes) {
    char operation;
    int arg_left, arg_right;
    double result;
    memcpy(&operation, bytes, sizeof(char));
    memcpy(&arg_left, bytes + sizeof(char), sizeof(int));
    memcpy(&arg_right, bytes + sizeof(char) + sizeof(int), sizeof(int));
    memcpy(&result, bytes + sizeof(char) + 2 * sizeof(int), sizeof(double));
    return Calculation(operation, arg_left, arg_right, result);
}

char Calculation::GetOperation() {
    return this->operation;
}

int Calculation::GetArgLeft() {
    return this->arg_left;
}

int Calculation::GetArgRight() {
    return this->arg_right;
}

double Calculation::GetResult() {
    return this->result;
}

void Calculation::SetResult(double result) {
    this->result = result;
}