#include <cstring>
#include <cstdio>
#include "include/message.h"

char* Calculation::Serialize() const {
    char* result = new char[sizeof(char) + sizeof(int) + sizeof(int)];
    memcpy(result, &(this->operation), sizeof(char));
    memcpy(result + sizeof(char), &(this->arg_left), sizeof(int));
    memcpy(result + sizeof(char) + sizeof(int), &(this->arg_right), sizeof(int));
    return result;
}

Calculation Calculation::Deserialize(char* bytes) {
    char operation;
    int arg_left, arg_right;
    memcpy(&operation, bytes, sizeof(char));
    memcpy(&arg_left, bytes + sizeof(char), sizeof(int));
    memcpy(&arg_right, bytes + sizeof(char) + sizeof(int), sizeof(int));
    return Calculation(operation, arg_left, arg_right);
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

char* Response::Serialize() const {
    char* result = new char[sizeof(int)];
    memcpy(result, &(this->id), sizeof(int));
    return result;
}

Response Response::Deserialize(char* bytes) {
    int id;
    memcpy(&id, bytes, sizeof(int));
    return Response(id);
}

int Response::GetId() {
    return this->id;
}


char* Result::Serialize() const {
    char* result = new char[sizeof(double)];
    memcpy(result, &(this->value), sizeof(double));
    return result;
}

Result Result::Deserialize(char* bytes) {
    double value;
    memcpy(&value, bytes, sizeof(double));
    return Result(value);
}

double Result::GetValue() {
    return this->value;
}