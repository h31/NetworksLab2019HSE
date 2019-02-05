//
// Created by mikhail on 03.02.19.
//

#ifndef NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H
#define NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H


#include "Client.h"

class CurrencyClientApplication {
public:
    void run();

    CurrencyClientApplication(const std::string &host, uint16_t portno);

private:
    std::string host;
    uint16_t portno;
    Client client = Client(host, portno);

    void printUsage();
};


#endif //NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H
