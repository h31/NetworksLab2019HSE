#include <utility>

#include "client.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>

#include <string.h>


Client::Client(char* host_name, int port_number){
	this->host_name = host_name;
	this->port_number = port_number;
}

bool Client::connect(){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        return false;
    }

    struct hostent *server = gethostbyname(host_name);

    if (server == nullptr) {
        return false;
    }

    struct sockaddr_in serv_addr{};
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port_number);
    if (::connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    return true;
}


bool Client::sendRequest(){
    std::cout << "0 -- add user\n1 -- get all wallets\n2 -- sent to user\n3 -- ask from user\n"
         << "4 -- confirm sent request\n5 -- check money\n6 -- check sent requests\n7 -- disconnect\n\n Enter message type: ";

    int32_t message_type;
    std::cin >> message_type;
    
    bzero(buffer, 128);
    std::memcpy(buffer, &message_type, 4);

    switch(message_type) {
        case 0: 
            addUser();
            break;
        case 1:
            break;
        case 2: 
            sendMoney();
            break;
        case 3: 
            requestMoney();
            break;
        case 4:
        	confirmRequest();
            break;
        case 5: 
            checkMoney();
            break;
        case 6:
            checkRequests(); 
            break;
        case 7:
            return false;
        default:
            std::cout << "wrong message type";
            return false;
    } 

    ssize_t n = write(sockfd, buffer, strlen(buffer));

    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    return true;
}

void Client::getResponse() {
    bzero(buffer, 256);
    ssize_t n = read(sockfd, buffer, 255);

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    int32_t message_type = shortNumberResponse(0);

    switch(message_type) {
        case 0: 
            addUserRerponse();
            break;
        case 1:
            getAllWalletsResponse();
            break;
        case 2: 
            successResponse();
            break;
        case 3: 
            successResponse();
            break;
        case 4:
        	successResponse();
            break;
        case 5: 
            std::cout << "You have " << longNumberResponse(4);
            break;
        case 6:
            checkRequestsResponse(); 
            break;
        default:
            std::cout << "wrong message type";
            return;
    }
}  

void Client::addUser() {
	readPassword(4);
}

void Client::sendMoney() {
	readClientWallet(4); 
    readWallet(4 + 8, "Enter wallet number to send money: ");
    readPassword(4 + 2 * 8);
    readMoney(4 + 2 * 8 + 32);
}

void Client::requestMoney() {
    readClientWallet(4);
    readWallet(4 + 8, "Enter wallet number to ask money: ");
    readPassword(4 + 2 * 8);
    readMoney(4 + 2 * 8 + 32);
}

void Client::confirmRequest() {
	readClientWallet(4);
    readRequestId(4 + 8);
    readPassword(4 + 2 * 8);
    readConfirm(4 + 2 * 8 + 32);
}

void Client::checkMoney() {
	readClientWallet(4);
    readPassword(4 + 8);
}

void Client::checkRequests() {
	readClientWallet(4);
    readPassword(4 + 8);
}

void Client::readPassword(int offset) {
	std::string password;    
    while(true) {
		std::cout << "Enter password: ";
        std::cin >> password;
        if (password.size() > 32) {
            std::cout << "Too long";
        } else {
        	break;
        }
    }
    char const *c = password.c_str();
    std::memcpy(buffer + offset, c, 32);
}

void Client::readClientWallet(int offset) {
	readWallet(offset, "Enter your wallet number: ");
}

void Client::readWallet(int offset, std::string whoseWallet) {
	std::cout << whoseWallet;
    int64_t walletNumber;
    std::cin >> walletNumber;
    std::memcpy(buffer + offset, &walletNumber, 8);
}

void Client::readMoney(int offset) {
    std::cout << "Enter how much to send: ";
    int32_t money;
    std::cin >> money;
    std::memcpy(buffer + offset, &money, 4);
}

void Client::readConfirm(int offset) {
    std::cout << "Do you want to confirm request?\n 1 - yes\n 0 - no\n: ";
    int32_t confirmation;
    std::cin >> confirmation;
    std::memcpy(buffer + offset, &confirmation, 4);
}

void Client::readRequestId(int offset) {
	std::cout << "Enter request ID: ";
    int64_t walletNumber;
    std::cin >> walletNumber;
    std::memcpy(buffer + offset, &walletNumber, 8);
}

void Client::addUserRerponse() {
	successResponse();
	int64_t number = longNumberResponse(8);
	std::cout << "Your wallet number: " << number << "\n";
}

int64_t Client::longNumberResponse(int offset) {
    return int64_t((unsigned char)(buffer[offset]) << 56 |
            (unsigned char)(buffer[offset + 1]) << 48 |
            (unsigned char)(buffer[offset + 2]) << 40 |
            (unsigned char)(buffer[offset + 3]) << 32 |
            (unsigned char)(buffer[offset + 4]) << 24 |
            (unsigned char)(buffer[offset + 5]) << 16 |
            (unsigned char)(buffer[offset + 6]) << 8 |
            (unsigned char)(buffer[offset + 7]));
}

int32_t Client::shortNumberResponse(int offset) {
    return int32_t((unsigned char)(buffer[offset]) << 24 |
            (unsigned char)(buffer[offset + 1]) << 16 |
            (unsigned char)(buffer[offset + 2]) << 8 |
            (unsigned char)(buffer[offset + 3]));
}

void Client::getAllWalletsResponse() {
    int64_t walletsNumber = longNumberResponse(4);
    for (int i = 0; i < walletsNumber; i++) {
    	std::cout << longNumberResponse(4 + 8 * (i + 1)) << "\n";
    }
}


void Client::successResponse() {
	int32_t success = shortNumberResponse(4);
	if (success == 1) {
		std::cout << "Success\n";
	} else {
		std::cout << "Fail\n";
	}
}

void Client::checkRequestsResponse() {
	int64_t requestNumber = longNumberResponse(4);
    for (int i = 0; i < requestNumber; i++) {
    	std::cout << "id: " << longNumberResponse(8 + 24 * i) 
    	<< " from: " << longNumberResponse(8 + 24 * i + 8) 
    	<< "money: " << longNumberResponse(8 + 24 * i + 16) << "\n";
    }
}