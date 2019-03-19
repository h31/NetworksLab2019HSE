//
// Created by karvozavr on 16/02/19.
//

#include "CalcuatorServerDriver.hpp"

CalcuatorServerDriver::~CalcuatorServerDriver() {
    shutdown(m_socket, SHUT_RDWR);
    close(m_socket);
}

void CalcuatorServerDriver::connect() {
    m_socket = connectToServer(m_host, m_port);
    m_readingThread = std::thread(&CalcuatorServerDriver::readingThreadTask, this);
}

void CalcuatorServerDriver::readingThreadTask() {
    while (true) {
        auto response = readObject<CalculatorResponse>(m_socket);
        if (response.errorCode != WAIT_FOR_RESULT) {
            if (response.operationType == SLOW) {
                m_longResults.push(response);
            } else if (response.operationType == FAST) {
                m_instantResults.push(response);
            }
        }
    }
}

bool CalcuatorServerDriver::hasResult() {
    return !m_longResults.empty();
}

CalculatorResponse CalcuatorServerDriver::getResult() {
    return m_longResults.pop();
}

void CalcuatorServerDriver::factorial(uint32_t id, int64_t arg) {
    sendRequest({FACT, id, arg, 0});
}

void CalcuatorServerDriver::sqrt(uint32_t id, int64_t arg) {
    sendRequest({SQRT, id, arg, 0});
}

CalculatorResponse CalcuatorServerDriver::plus(uint32_t id, int64_t arg1, int64_t arg2) {
    sendRequest({PLUS, id, arg1, arg2});
    return getResponse();
}

CalculatorResponse CalcuatorServerDriver::minus(uint32_t id, int64_t arg1, int64_t arg2) {
    sendRequest({MINUS, id, arg1, arg2});
    return getResponse();
}

CalculatorResponse CalcuatorServerDriver::multiply(uint32_t id, int64_t arg1, int64_t arg2) {
    sendRequest({MULT, id, arg1, arg2});
    return getResponse();
}

CalculatorResponse CalcuatorServerDriver::divide(uint32_t id, int64_t arg1, int64_t arg2) {
    sendRequest({DIV, id, arg1, arg2});
    return getResponse();
}

void CalcuatorServerDriver::sendRequest(CalculatorRequest const &request) {
    writeObject(m_socket, request);
}

CalculatorResponse CalcuatorServerDriver::getResponse() {
    while (m_instantResults.empty()) {}
    return m_instantResults.pop();
}


