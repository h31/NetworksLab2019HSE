#include <utility>

#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <unordered_set>
#include "requests.hpp"
#include "socketUtils.hpp"
#include "ConcurrentQueue.hpp"
#include "ConcurrentMap.hpp"

class CalcuatorServerDriver {
public:
    CalcuatorServerDriver(std::string host, uint16_t port) : m_host_str(std::move(host)), m_port(port) {}

    ~CalcuatorServerDriver();

    void connect();

    bool hasResult();

    CalculatorResponse getResult();

    void factorial(uint32_t id, int64_t arg);

    void sqrt(uint32_t id, int64_t arg);

    CalculatorResponse plus(uint32_t id, int64_t arg1, int64_t arg2);

    CalculatorResponse minus(uint32_t id, int64_t arg1, int64_t arg2);

    CalculatorResponse multiply(uint32_t id, int64_t arg1, int64_t arg2);

    CalculatorResponse divide(uint32_t id, int64_t arg1, int64_t arg2);

    void results();

private:
    void sendRequest(CalculatorRequest const &request);

    CalculatorResponse getResponse(CalculatorRequest const &request);

    void readingThreadTask();

    ConcurrentQueue<CalculatorResponse> m_longResults;
    ConcurrentMap<uint32_t, CalculatorResponse> m_instantResults;
    int m_socket = 0;
    std::string m_host_str;
    sockaddr_in m_host;
    uint16_t m_port;
    std::thread m_readingThread;
    std::unordered_set<uint32_t> m_longOperations;
};
