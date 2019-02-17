#pragma once

#include <iostream>
#include <sstream>
#include <iterator>
#include "CalcuatorServerDriver.hpp"
#include "socketUtils.hpp"

class CalculatorApp {
public:
    CalculatorApp(std::string const &host, uint16_t port) : m_driver(host, port) {}

    void start();

private:
    void printPrompt(uint32_t computationId);

    void processInput(std::string &line);

    template<typename T>
    void printResult(uint32_t id, T const &value) {
        std::cout << "Out [" << id << "]: " << value << std::endl;
    }

    void printEntryMessage();

    void printLine(std::string const &line = "");

    uint32_t m_currentComputation = 0;
    CalcuatorServerDriver m_driver;

    void printResponse(const CalculatorResponse &response);
};