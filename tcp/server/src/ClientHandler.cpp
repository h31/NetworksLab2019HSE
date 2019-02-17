

//
// Created by Olga Alehina on 2019-02-10.
//

#include "../include/ClientHandler.h"

ReadWriteHelper ClientHandler::readWriteHelper;

void ClientHandler::operator()(int clientSocketFd) {
    if (clientSocketFd < 0) {
        throw ClientHandlerException("ERROR on accept");
    }
    ssize_t countRead;
    uint8_t buffer[256];
    while (true) {
        bzero(buffer, 256);

        countRead = read(clientSocketFd, buffer, sizeof(buffer));

        if (countRead < 4) {
            break;
        }

        uint32_t requestType = readWriteHelper.get4Bytes(buffer, 0);

        ssize_t n;
        switch (requestType) {
            case 0: {
                Response response = addWallet(buffer, requestType, countRead);
                n = write(clientSocketFd, response.buffer, response.count);
                break;
            }
            case 1: {
                Response response = getWalletNumbers(requestType);
                n = write(clientSocketFd, response.buffer, response.count);
                break;
            }
            case 2: {
                Response response = sendTransfer(buffer, requestType, countRead);
                n = write(clientSocketFd, response.buffer, response.count);
                break;
            }
            case 3: {
                Response response = requestTransfer(buffer, requestType, countRead);
                n = write(clientSocketFd, response.buffer, response.count);
                break;
            }
            case 4: {
                Response response = acceptTransfer(buffer, requestType, countRead);
                n = write(clientSocketFd, response.buffer, response.count);
                break;
            }
            case 5: {
                Response response = getWalletInfo(buffer, requestType, countRead);
                n = write(clientSocketFd, response.buffer, response.count);
                break;
            }
            case 6: {
                Response response = getRequests(buffer, requestType, countRead);
                n = write(clientSocketFd, response.buffer, response.count);
                break;
            }
            default : {
                n = write(clientSocketFd, "I got your message", 18);
                break;
            }
        }
        if (n < 0) {
            perror("ERROR writing to socket");
            break;
        }
    }

}

Response ClientHandler::addWallet(uint8_t *inputBuffer, uint32_t type, ssize_t countRead) {
    uint8_t buffer[256];
    bzero(buffer, 256);

    //записываем тип
    readWriteHelper.set4Bytes(buffer, 0, type);
    int offset = 4;
    uint32_t isSuccess = 0;
    std::uint64_t walletNumber = 0;
    if (countRead < 36) {
        isSuccess = 0;
    } else {
        char password[32];
        bzero(password, 32);
        int offsetForChar = 4;
        for (char &i : password) {
            i = (char) readWriteHelper.getByte(inputBuffer, offsetForChar);
            offsetForChar++;
        }
        mutex_.lock();
        walletNumber = data.getFreeNumber();
        Wallet wallet(walletNumber, password, 100);
        ClientHandler::data.addNewWaller(wallet);
        mutex_.unlock();
        std::cout << (password);
        isSuccess = 1;
    }

    //записали результат
    readWriteHelper.set4Bytes(buffer, offset, isSuccess);
    offset += 4;

    //записали номер
    readWriteHelper.set8Bytes(buffer, offset, walletNumber);

    return {buffer, 256};
}

Response ClientHandler::getWalletNumbers(uint32_t type) {

    mutex_.lock_shared();
    std::set<uint64_t> walletNumbers = ClientHandler::data.getNumbers();
    mutex_.unlock_shared();

    uint8_t buffer[1024];
    bzero(buffer, 1024);

    // записываем тип
    readWriteHelper.set4Bytes(buffer, 0, type);
    int offset = 4;

    //записываем кол-во кошельков
    readWriteHelper.set8Bytes(buffer, offset, walletNumbers.size());
    offset += 8;

    //записываем все номера кошельков
    for (auto walletNumber : walletNumbers) {
        if (offset > 1024) {
            break;
        }
        readWriteHelper.set8Bytes(buffer, offset, walletNumber);
        offset += 8;
    }

    return {buffer, 1024};
}


Response ClientHandler::sendTransfer(uint8_t *inputBuffer, uint32_t type, ssize_t countRead) {
    uint8_t buffer[256];
    bzero(buffer, 256);
    readWriteHelper.set4Bytes(buffer, 0, type);
    int writeOffset = 4;
    int readOffset = 4;
    if (countRead < 4 + 8 + 8 + 32 + 8) {
        return Response(buffer, 256);
    }
    uint64_t numFrom = readWriteHelper.get8Bytes(inputBuffer, readOffset);
    readOffset += 8;
    uint64_t numTo = readWriteHelper.get8Bytes(inputBuffer, readOffset);
    readOffset += 8;
    char password[32];
    bzero(password, 32);
    for (char &i : password) {
        i = (char) readWriteHelper.getByte(inputBuffer, readOffset);
        readOffset++;
    }
    uint64_t countMoney = readWriteHelper.get8Bytes(inputBuffer, readOffset);


    int32_t isSuccess = trassfer(numFrom, numTo, password, countMoney);
    readWriteHelper.set4Bytes(buffer, writeOffset, isSuccess);
    data.print();
    return Response(buffer, 256);
}

Response ClientHandler::requestTransfer(uint8_t *inputBuffer, uint32_t type, ssize_t countRead) {
    uint8_t buffer[256];
    bzero(buffer, 256);
    readWriteHelper.set4Bytes(buffer, 0, type);
    int writeOffset = 4;
    int readOffset = 4;
    uint32_t isSuccess = 0;
    if (countRead < 4 + 8 + 8 + 32 + 8) {
        return Response(buffer, 256);
    }
    uint64_t numFrom = readWriteHelper.get8Bytes(inputBuffer, readOffset);
    readOffset += 8;
    uint64_t numTo = readWriteHelper.get8Bytes(inputBuffer, readOffset);
    readOffset += 8;
    char password[32];
    bzero(password, 32);
    for (char &i : password) {
        i = (char) readWriteHelper.getByte(inputBuffer, readOffset);
        readOffset++;
    }
    uint64_t countMoney = readWriteHelper.get8Bytes(inputBuffer, readOffset);

    mutex_.lock_shared();
    if (countMoney < 0
        || !ClientHandler::data.isExist(numFrom, password)) {
        mutex_.unlock_shared();
        return Response(buffer, 256);
    }
    mutex_.unlock_shared();

    historyMutex.lock();
    ClientHandler::data.addHistoryEvents(numFrom, numTo, countMoney, REMITTANCE_REQUEST);
    historyMutex.unlock();
    isSuccess = 1;
    readWriteHelper.set4Bytes(buffer, writeOffset, isSuccess);
    data.print();
    return Response(buffer, 256);
}

Response ClientHandler::acceptTransfer(uint8_t *inputBuffer, uint32_t type, ssize_t countRead) {
    uint8_t buffer[256];
    bzero(buffer, 256);
    readWriteHelper.set4Bytes(buffer, 0, type);
    int writeOffset = 4;
    int readOffset = 4;
    if (countRead < 4 + 8 + 8 + 32 + 4) {
        return Response(buffer, 256);
    }
    uint64_t numFrom = readWriteHelper.get8Bytes(inputBuffer, readOffset);
    readOffset += 8;
    uint64_t historyId = readWriteHelper.get8Bytes(inputBuffer, readOffset);
    readOffset += 8;
    char password[32];
    bzero(password, 32);
    for (char &i : password) {
        i = (char) readWriteHelper.getByte(inputBuffer, readOffset);
        readOffset++;
    }
    std::string passwordString(password);
    uint32_t isAccept = readWriteHelper.get4Bytes(inputBuffer, readOffset);
    uint32_t isSuccess = 1;
    historyMutex.lock();
    isSuccess &= ClientHandler::data.setHistoryStatus(historyId, true);
    HistoryEvent event = ClientHandler::data.getEvent(historyId);
    if (event.eventType == NOTHING) {
        return Response(buffer, 256);
    }
    historyMutex.unlock();

    if (isAccept == 1) {
        isSuccess &= trassfer(numFrom, event.walletNumberTo, password, event.count);
    }
    readWriteHelper.set4Bytes(buffer, writeOffset, isSuccess);
    return Response(buffer, 256);
}

bool ClientHandler::trassfer(uint64_t numFrom, uint64_t numTo, std::string password, uint64_t countMoney) {
    mutex_.lock();
    if (countMoney < 0
        || !ClientHandler::data.isExist(numFrom, password)
        || ClientHandler::data.getMoney(numFrom, password) < countMoney) {
        mutex_.unlock();
        return false;
    }

    bool isSuccess = true;
    isSuccess &= ClientHandler::data.decMoney(numFrom, countMoney);
    isSuccess &= ClientHandler::data.incMoney(numTo, countMoney);
    mutex_.unlock();
    return isSuccess;
}

Response ClientHandler::getWalletInfo(uint8_t *inputBuffer, uint32_t type, ssize_t countRead) {
    uint8_t buffer[256];
    bzero(buffer, 256);
    readWriteHelper.set4Bytes(buffer, 0, type);
    int writeOffset = 4;
    int readOffset = 4;
    if (countRead < 4 + 8 + 32) {
        return Response(buffer, 256);
    }
    //input: <5: 4 байта><номер кошелька: 8 байт><пароль: 32 байт>
    //output: <5: 4 байта><бабло: 8 байт>

    uint64_t numWallet = readWriteHelper.get8Bytes(inputBuffer, readOffset);
    readOffset += 8;
    char password[32];
    bzero(password, 32);
    for (char &i : password) {
        i = (char) readWriteHelper.getByte(inputBuffer, readOffset);
        readOffset++;
    }
    mutex_.lock_shared();
    uint64_t countMoney = 0;
    if (data.isExist(numWallet, password)) {
        countMoney = data.getMoney(numWallet, password);
    }
    mutex_.unlock();
    readWriteHelper.set8Bytes(buffer, writeOffset, countMoney);
    return Response(buffer, 256);
}

Response ClientHandler::getRequests(uint8_t *inputBuffer, uint32_t type, ssize_t countRead) {
    //input: <6: 4 байта>
    // <номер кошелька: 8 байт>
    // <пароль: 32 байт>
    //output: <6: 4 байта><количество запросов: 8 байт><id запроса: 8 байт><от кого: 8 байт><сколько: 8 байт>...

    uint8_t buffer[1024];
    bzero(buffer, 1024);
    readWriteHelper.set4Bytes(buffer, 0, type);
    int writeOffset = 4;
    int readOffset = 4;
    if (countRead < 4 + 8 + 32) {
        return Response(buffer, 1024);
    }

    uint64_t numWallet = readWriteHelper.get8Bytes(inputBuffer, readOffset);
    readOffset += 8;
    char password[32];
    bzero(password, 32);
    for (char &i : password) {
        i = (char) readWriteHelper.getByte(inputBuffer, readOffset);
        readOffset++;
    }

    mutex_.lock_shared();
    if (!data.isExist(numWallet, password)) {
        return {buffer, 1024};
    }
    mutex_.unlock();

    historyMutex.lock_shared();
    std::set<HistoryEvent> events = data.getRequests(numWallet);
    historyMutex.unlock_shared();

    uint64_t requestCount = events.size();
    readWriteHelper.set8Bytes(buffer, writeOffset, requestCount);
    writeOffset += 8;
    for (HistoryEvent event : events) {
        if (writeOffset < 1024 - 24) {
            readWriteHelper.set8Bytes(buffer, writeOffset, event.id);
            writeOffset += 8;
            readWriteHelper.set8Bytes(buffer, writeOffset, event.walletNumberFrom);
            writeOffset += 8;
            readWriteHelper.set8Bytes(buffer, writeOffset, event.count);
            writeOffset += 8;
        }
    }
    return Response(buffer, 1024);
}


