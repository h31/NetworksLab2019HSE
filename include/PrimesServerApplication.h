//
// Created by mikhail on 03.02.19.
//

#ifndef NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H
#define NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H


#include "Server.h"

class PrimesServerApplication {
public:
    void run();

    explicit PrimesServerApplication(uint16_t portno);
    PrimesServerApplication();

private:
    const static uint16_t DEFAULT_PORTNO = 5001;
    uint16_t portno;
    Server server;

    void printUsage();
};


#endif //NETWORKSLAB2019HSE_CURRENCYCLIENTAPPLICATION_H
