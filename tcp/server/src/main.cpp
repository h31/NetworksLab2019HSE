#include <iostream>
#include "../include/PaymentSystemServer.h"

int main(int argc, char *argv[]) {
    int portNo;

    if (argc < 2) {
        std::cout << "Введите номер порта для сервера платежной системы" << std::endl;
        std::cin >> portNo;
    } else {
        portNo = (uint16_t) std::stoi(argv[2]);
    }

    PaymentSystemServer paymentSystemServer;

    paymentSystemServer.run(portNo);
    return 0;
}