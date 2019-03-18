#include <socketUtils.hpp>
#include <CalculatorApp.hpp>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    error("Invalid argumets", "Usage: calculator <HOST> <PORT>");
  }

  CalculatorApp app(std::string(argv[1]), static_cast<uint16_t>(atoi(argv[2])));
  app.start();
}