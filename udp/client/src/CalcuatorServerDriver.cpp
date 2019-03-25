#include "CalcuatorServerDriver.hpp"

CalcuatorServerDriver::~CalcuatorServerDriver() {
    shutdown(m_socket, SHUT_RDWR);
    close(m_socket);
}

void CalcuatorServerDriver::connect() {
    m_socket = socketInit();
    m_host = hostInit(m_host_str, m_port);
    m_readingThread = std::thread(&CalcuatorServerDriver::readingThreadTask, this);
}

void CalcuatorServerDriver::readingThreadTask() {
    while (true) {
        auto response = readObject<CalculatorResponse>(m_host, m_socket);
        if (response.errorCode != WAIT_FOR_RESULT && response.operationType == SLOW) {
            m_longResults.push(response);
        }
        m_instantResults.put(response.computationId, response);
    }
}

bool CalcuatorServerDriver::hasResult() {
    return !m_longResults.empty();
}

CalculatorResponse CalcuatorServerDriver::getResult() {
    auto response = m_longResults.pop();
    m_longOperations.erase(response.computationId);
    return response;
}

void CalcuatorServerDriver::factorial(uint32_t id, int64_t arg) {
    getResponse({FACT, id, arg, 0});
    m_longOperations.insert(id);
}

void CalcuatorServerDriver::sqrt(uint32_t id, int64_t arg) {
    getResponse({SQRT, id, arg, 0});
    m_longOperations.insert(id);
}

CalculatorResponse CalcuatorServerDriver::plus(uint32_t id, int64_t arg1, int64_t arg2) {
    return getResponse({PLUS, id, arg1, arg2});
}

CalculatorResponse CalcuatorServerDriver::minus(uint32_t id, int64_t arg1, int64_t arg2) {
    return getResponse({MINUS, id, arg1, arg2});
}

CalculatorResponse CalcuatorServerDriver::multiply(uint32_t id, int64_t arg1, int64_t arg2) {
    return getResponse({MULT, id, arg1, arg2});
}

CalculatorResponse CalcuatorServerDriver::divide(uint32_t id, int64_t arg1, int64_t arg2) {
    return getResponse({DIV, id, arg1, arg2});
}

void CalcuatorServerDriver::results() {
    for (uint32_t id : m_longOperations) {
        getResponse({LONG_OP_RESULT, id, 0, 0});
    }
}

void CalcuatorServerDriver::sendRequest(CalculatorRequest const &request) {
    writeObject(m_host, m_socket, request);
}

CalculatorResponse CalcuatorServerDriver::getResponse(CalculatorRequest const &request) {
    sendRequest(request);
    int i = 0;
    int resend = 0;

    while (!m_instantResults.contains(request.computationId)) {
        if (i == 10) {
            if (resend == 5) {
                error("get response", "Connection error.");
            }
            sendRequest(request);
            resend++;
            i = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        i++;
    }

    return m_instantResults.pop(request.computationId);
}



