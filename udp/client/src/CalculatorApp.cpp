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

    if (results.size() <= 3) {
        if (results.size() == 2) {
            if (results[0] == "fact") {
                m_driver.factorial(m_currentComputation, static_cast<int64_t>(std::stoull(results[1])));
            } else if (results[0] == "sqrt") {
                m_driver.sqrt(m_currentComputation, static_cast<int64_t>(std::stoull(results[1])));
            } else {
                std::cout << "Syntax error." << std::endl;
            }
        } else if (results.size() == 1) {
            if (results[0] == "rslt") {
                m_driver.results();
            } else {
                std::cout << "Syntax error." << std::endl;
            }
        } else if (results.size() == 3 && results[0].length() == 1) {
            CalculatorResponse response{};
            switch (results[0][0]) {
                case '+':
                    response = m_driver.plus(m_currentComputation,
                                             static_cast<int64_t>(std::stoull(results[1])),
                                             static_cast<int64_t>(std::stoull(results[2])));
                    break;
                case '-':
                    response = m_driver.minus(m_currentComputation,
                                              static_cast<int64_t>(std::stoull(results[1])),
                                              static_cast<int64_t>(std::stoull(results[2])));
                    break;
                case '*':
                    response = m_driver.multiply(m_currentComputation,
                                                 static_cast<int64_t>(std::stoull(results[1])),
                                                 static_cast<int64_t>(std::stoull(results[2])));
                    break;
                case '/':
                    response = m_driver.divide(m_currentComputation,
                                               static_cast<int64_t>(std::stoull(results[1])),
                                               static_cast<int64_t>(std::stoull(results[2])));
                    break;
                default:
                    std::cout << "Syntax error." << std::endl;
                    break;
            }

            printResponse(response);
        } else {
            std::cout << "Syntax error." << std::endl;
        }
    } else {
        std::cout << "Syntax error." << std::endl;
    }

    printLine();

    while (m_driver.hasResult()) {
        printResponse(m_driver.getResult());
        printLine();
    }
}

void CalculatorApp::printResponse(const CalculatorResponse &response) {
    if (response.errorCode != OK) {
        printResult(response.computationId, errorCodeToString(response.errorCode));
    } else {
        printResult(response.computationId, response.result);
    }
}

void CalculatorApp::printEntryMessage() {
    std::cout
            << "Welcome!\n"
            << "\tOnline calculator 1.0\n\n"
            << "\tSupported operations: + - * / fact sqrt rslt quit\n"
            << "\tUse prefix notation (e.g + 2 3).\n"
            << std::endl;
}

void CalculatorApp::printLine(const std::string &line) {
    std::cout << line << std::endl;
}
