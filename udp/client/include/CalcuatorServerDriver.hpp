#include <utility>

#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include "requests.hpp"
#include "socketUtils.hpp"
#include "ConcurrentQueue.hpp"
#include "ConcurrentMap.hpp"
#include "PacketManager.hpp"

class CalcuatorServerDriver {
public:
  CalcuatorServerDriver(std::string host, uint16_t port) : m_host(std::move(host)), m_port(port) {}

  ~CalcuatorServerDriver();

  void initialize();

  bool hasResult();

  CalculatorResponse getResult();

  void factorial(uint32_t id, int64_t arg);

  void sqrt(uint32_t id, int64_t arg);

  CalculatorResponse plus(uint32_t id, int64_t arg1, int64_t arg2);

  CalculatorResponse minus(uint32_t id, int64_t arg1, int64_t arg2);

  CalculatorResponse multiply(uint32_t id, int64_t arg1, int64_t arg2);

  CalculatorResponse divide(uint32_t id, int64_t arg1, int64_t arg2);

private:
  void sendRequest(CalculatorRequest const &request);

  void sendRequestImpl(CalculatorRequest const &request);

  void checkForComputations();

  CalculatorResponse getResponse(uint32_t computationId);

  void readingThreadTask();

  ConcurrentQueue<CalculatorResponse> m_longResults;
  ConcurrentMap<uint32_t, CalculatorResponse> m_instantResults;
  int m_socket = 0;
  std::string m_host;
  uint16_t m_port;
  sockaddr_in m_server;
  std::thread m_readingThread;
  PacketManager m_packetManager;
  CalculatorRequest m_lastRequest{};
  std::vector<uint32_t> m_longComputations;
  volatile bool m_terminated = false;
};
