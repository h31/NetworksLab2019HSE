#include "CalculatorApp.hpp"

void CalculatorApp::start() {
    m_driver.connect();

    printEntryMessage();

    std::string line;
    printPrompt(m_currentComputation);

    while (std::getline(std::cin, line)) {
        processInput(line);
        ++m_currentComputation;
        printPrompt(m_currentComputation);
    }
}

void CalculatorApp::printPrompt(uint32_t computationId) {
    std::cout << "In [" << computationId << "]: ";
}

void CalculatorApp::processInput(std::string &line) {
    std::istringstream iss(line);
    std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());

    if (results.size() == 2 || results.size() == 3 || results.empty()) {
        if (results.size() == 2) {
            if (results[0] == "fact") {
                m_driver.factorial(m_currentComputation, static_cast<int64_t>(std::stoull(results[1])));
            } else if (results[0] == "sqrt") {
                m_driver.sqrt(m_currentComputation, static_cast<int64_t>(std::stoull(results[1])));
            }
        }
        if (results.size() == 3 && results[0].length() == 1) {
            switch (results[0][0]) {
                case '+':
                    m_driver.plus(m_currentComputation,
                                  static_cast<int64_t>(std::stoull(results[1])),
                                  static_cast<int64_t>(std::stoull(results[2])));
                    break;
                case '-':
                    m_driver.minus(m_currentComputation,
                                   static_cast<int64_t>(std::stoull(results[1])),
                                   static_cast<int64_t>(std::stoull(results[2])));
                    break;
                case '*':
                    m_driver.multiply(m_currentComputation,
                                      static_cast<int64_t>(std::stoull(results[1])),
                                      static_cast<int64_t>(std::stoull(results[2])));
                    break;
                case '/':
                    m_driver.divide(m_currentComputation,
                                    static_cast<int64_t>(std::stoull(results[1])),
                                    static_cast<int64_t>(std::stoull(results[2])));
                    break;
                default:
                    std::cout << "Syntax error." << std::endl;
                    break;
            }
        }
    } else {
        std::cout << "Syntax error." << std::endl;
    }

    printLine();

    while (m_driver.hasResult()) {
        auto result = m_driver.getResult();
        if (result.errorCode != 0) {
            printResult(result.computationId, errorCodeToString(result.errorCode));
        } else {
            printResult(result.computationId, result.result);
        }
        printLine();
    }
}

void CalculatorApp::printEntryMessage() {
    std::cout << "Welcome!\n\tOnline calculator 1.0\n\n\n\n\tSupported operations: " << std::endl;
}

void CalculatorApp::printLine(const std::string &line) {
    std::cout << line << std::endl;
}
