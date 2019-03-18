//
// Created by karvozavr on 16/02/19.
//

#include "CalcuatorServerDriver.hpp"

CalcuatorServerDriver::~CalcuatorServerDriver() {
  m_terminated = true;
  shutdown(m_socket, SHUT_RDWR);
  close(m_socket);
}

void CalcuatorServerDriver::initialize() {
  m_socket = initializeSocket();
  m_server = initializeHost(m_host, m_port);
  m_readingThread = std::thread(&CalcuatorServerDriver::readingThreadTask, this);
  m_packetManager = PacketManager(m_server, m_port, m_socket);
}

void CalcuatorServerDriver::readingThreadTask() {
  while (!m_terminated) {
    CalculatorResponse response{};
    m_packetManager.receiveMessage(reinterpret_cast<uint8_t *>(&response), sizeof(response));

    if (response.errorCode != WAIT_FOR_RESULT) {
      if (response.type == SLOW) {
        m_longResults.push(response);
      } else if (response.type == FAST) {
        m_instantResults.add(response.computationId, response);
      }
    } else {
      m_instantResults.add(response.computationId, response);
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
  getResponse(id);
}

void CalcuatorServerDriver::sqrt(uint32_t id, int64_t arg) {
  sendRequest({SQRT, id, arg, 0});
  getResponse(id);
}

CalculatorResponse CalcuatorServerDriver::plus(uint32_t id, int64_t arg1, int64_t arg2) {
  sendRequest({PLUS, id, arg1, arg2});
  return getResponse(id);
}

CalculatorResponse CalcuatorServerDriver::minus(uint32_t id, int64_t arg1, int64_t arg2) {
  sendRequest({MINUS, id, arg1, arg2});
  return getResponse(id);
}

CalculatorResponse CalcuatorServerDriver::multiply(uint32_t id, int64_t arg1, int64_t arg2) {
  sendRequest({MULT, id, arg1, arg2});
  return getResponse(id);
}

CalculatorResponse CalcuatorServerDriver::divide(uint32_t id, int64_t arg1, int64_t arg2) {
  sendRequest({DIV, id, arg1, arg2});
  return getResponse(id);
}

void CalcuatorServerDriver::sendRequestImpl(CalculatorRequest const &request) {
  m_lastRequest = request;
  m_packetManager.sendMessage(reinterpret_cast<const uint8_t *>(&request), sizeof(request));
}

void CalcuatorServerDriver::sendRequest(CalculatorRequest const &request) {
  checkForComputations();
  sendRequestImpl(request);
}

CalculatorResponse CalcuatorServerDriver::getResponse(uint32_t computationId) {
  const int COUNTER_INITIAL = 5;
  const int MAX_TIMES_RESEND = 10;
  const int MILLISECONDS_WAIT = 100;

  int counter = COUNTER_INITIAL;
  int timesResend = 0;

  std::optional<CalculatorResponse> response = m_instantResults.get(computationId);

  while (!response.has_value()) {
    if (counter == 0) {
      if (timesResend == MAX_TIMES_RESEND) {
        error("Lost connection.");
      }
      sendRequest(m_lastRequest);
      ++timesResend;
      counter = COUNTER_INITIAL;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(MILLISECONDS_WAIT));
    response = m_instantResults.get(computationId);

    --counter;
  }

  return response.value();
}

void CalcuatorServerDriver::checkForComputations() {
  for (uint32_t id : m_longComputations) {
    sendRequestImpl({LONG_OP_RESULT, id, 0, 0});
    getResponse(id);
  }
}


