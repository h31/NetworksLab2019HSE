//
// Created by mikhail on 03.02.19.
//

#ifndef NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H
#define NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H


#include "Client.h"

class CurrencyClientApplication {
public:
    void run();

private:
    Client client = Client();

    void printUsage();
};


#endif //NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H
