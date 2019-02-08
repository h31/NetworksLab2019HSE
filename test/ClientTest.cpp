#define CATCH_CONFIG_MAIN

#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include "catch.hpp"

#include "Client.h"

class ClientTest {
public:
    uint16_t PORTNO = 0;
    const std::string HOST = "localhost";
};

uint16_t get_current_port(int sockfd);

void setup_server_returning(const std::vector<int8_t> &expected_to_receive,
        const std::vector<int8_t> &return_value, ClientTest &clientTest) {
    int sockfd, newsockfd;
    unsigned int clilen;
    char buffer[256];
    struct sockaddr_in serv_addr{}, cli_addr{};
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(0);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    clientTest.PORTNO = get_current_port(sockfd);
    listen(sockfd, 1);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    bzero(buffer, 256);
    ssize_t n_read = read(newsockfd, buffer, 255);
    if (n_read < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    std::vector<int8_t> received(buffer, buffer + n_read);
    REQUIRE(received == expected_to_receive);
    if (write(newsockfd, return_value.data(), return_value.size()) < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}

uint16_t get_current_port(int sockfd) {
    struct sockaddr_in serv_addr{};
    unsigned int clilen = sizeof(serv_addr);
    if (getsockname(sockfd, (struct sockaddr *) &serv_addr, &clilen) < 0) {
        perror("ERROR getting current port");
        exit(1);
    }
    return ntohs(serv_addr.sin_port);
}

TEST_CASE("list empty currencies") {
    ClientTest clientTest;
    std::vector<int8_t> expected_to_receive = {0, 0, 0, 0, '\\', 0};
    std::vector<int8_t> server_return = {'\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    std::vector<Currency> result = client.list();
    REQUIRE(result.empty());
    server_thread.join();
}

TEST_CASE("list one currency") {
    ClientTest clientTest;
    std::vector<int8_t> expected_to_receive = {0, 0, 0, 0, '\\', 0};
    std::vector<int8_t> server_return = {'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y','N', 'a', 'm', 'e', '1', 0, 0, 0,
                                         1, 1, 1, 1,
                                         1,
                                         1, 0, 1, 0,
                                         12, 0, 0, 0,
                                         '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    std::vector<Currency> result = client.list();
    std::vector<Currency> expected = {Currency("currencyName1",
                                      (1 << 24) + (1 << 16) + (1 << 8) + (1 << 0),
                                      (1 << 16) + (1 << 0),
                                      12)};
    REQUIRE(result == expected);
    server_thread.join();
}


TEST_CASE("list two currencies") {
    ClientTest clientTest;
    std::vector<int8_t> expected_to_receive = {0, 0, 0, 0, '\\', 0};
    std::vector<int8_t> server_return = {'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y','N', 'a', 'm', 'e', '1', 0, 0, 0,
                                         1, 1, 1, 1,
                                         1,
                                         1, 0, 1, 0,
                                         12, 0, 0, 0,
                                         'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y','N', 'a', 'm', 'e', '2', 'e', 'n', 'd',
                                         1, 0, 0, 0,
                                         0,
                                         0, 0, 0, 0,
                                         0, 0, 0, 0,
                                         '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    std::vector<Currency> result = client.list();
    std::vector<Currency> expected = {Currency("currencyName1",
                                               (1 << 24) + (1 << 16) + (1 << 8) + (1 << 0),
                                               (1 << 16) + (1 << 0),
                                               12),
                                      Currency("currencyName2end", 1)};
    REQUIRE(result == expected);
    server_thread.join();
}

TEST_CASE("list three currencies") {
    ClientTest clientTest;
    std::vector<int8_t> expected_to_receive = {0, 0, 0, 0, '\\', 0};
    std::vector<int8_t> server_return = {'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y','N', 'a', 'm', 'e', '1', 0, 0, 0,
                                         1, 1, 1, 1,
                                         1,
                                         1, 0, 1, 0,
                                         12, 0, 0, 0,
                                         'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y','N', 'a', 'm', 'e', '2', 'e', 'n', 'd',
                                         1, 0, 0, 0,
                                         0,
                                         0, 0, 0, 0,
                                         0, 0, 0, 0,
                                         'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y','N', 'a', 'm', 'e', '3', 0, 0, 0,
                                         1, 0, 1, 0,
                                         1,
                                         1, 0, 0, 0,
                                         1, 0, 0, 0,
                                         '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    std::vector<Currency> result = client.list();
    std::vector<Currency> expected = {Currency("currencyName1",
                                               (1 << 0) + (1 << 8) + (1 << 16) + (1 << 24),
                                               (1 << 0) + (1 << 16),
                                               12),
                                      Currency("currencyName2end", 1),
                                      Currency("currencyName3",
                                               (1 << 0) + (1 << 16),
                                               (1 << 0),
                                               (1 << 0))};
    REQUIRE(result == expected);
    server_thread.join();
}

TEST_CASE("add non-existing currency") {
    ClientTest clientTest;
    Currency currency_to_add("currencyName", (3 << 0) + (44 << 8) + (22 << 24));
    std::vector<int8_t> expected_to_receive = {1, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 0, 0, 0, 0,
                                               3, 44, 0, 22,
                                               '\\', 0};
    std::vector<int8_t> server_return = {1, '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    REQUIRE(client.addCurrency(currency_to_add));
    server_thread.join();
}

TEST_CASE("add existing currency") {
    ClientTest clientTest;
    Currency currency_to_add("currencyName", (3 << 0) + (44 << 8) + (22 << 24));
    std::vector<int8_t> expected_to_receive = {1, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 0, 0, 0, 0,
                                               3, 44, 0, 22,
                                               '\\', 0};
    std::vector<int8_t> server_return = {0, '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    REQUIRE(!client.addCurrency(currency_to_add));
    server_thread.join();
}

TEST_CASE("remove non-existing currency") {
    ClientTest clientTest;
    Currency currency_to_remove("currencyName");
    std::vector<int8_t> expected_to_receive = {2, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 0, 0, 0, 0,
                                               '\\', 0};
    std::vector<int8_t> server_return = {0, '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    REQUIRE(!client.remove(currency_to_remove));
    server_thread.join();
}

TEST_CASE("remove existing currency") {
    ClientTest clientTest;
    Currency currency_to_remove("currencyName");
    std::vector<int8_t> expected_to_receive = {2, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 0, 0, 0, 0,
                                               '\\', 0};
    std::vector<int8_t> server_return = {1, '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    REQUIRE(client.remove(currency_to_remove));
    server_thread.join();
}

TEST_CASE("add rate to non-existing currency") {
    ClientTest clientTest;
    Currency currency_to_add("currencyName");
    int32_t new_rate = (3 << 0) + (44 << 8) + (11 << 16) + (22 << 24);
    std::vector<int8_t> expected_to_receive = {3, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 0, 0, 0, 0,
                                               3, 44, 11, 22,
                                               '\\', 0};
    std::vector<int8_t> server_return = {0, '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    REQUIRE(!client.addRate(currency_to_add, new_rate));
    server_thread.join();
}

TEST_CASE("add rate to existing currency") {
    ClientTest clientTest;
    Currency currency_to_add("currencyNameExit");
    int32_t new_rate = (3 << 0) + (44 << 8) + (11 << 16) + (22 << 24);
    std::vector<int8_t> expected_to_receive = {3, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 'E', 'x', 'i', 't',
                                               3, 44, 11, 22,
                                               '\\', 0};
    std::vector<int8_t> server_return = {1, '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    REQUIRE(client.addRate(currency_to_add, new_rate));
    server_thread.join();
}

TEST_CASE("get empty history for non-existing currency") {
    ClientTest clientTest;
    Currency currency_to_get_history("currencyNameExit");
    std::vector<int8_t> expected_to_receive = {4, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 'E', 'x', 'i', 't',
                                               '\\', 0};
    std::vector<int8_t> server_return = {'\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    auto result = client.getCurrencyWithHistory(currency_to_get_history);
    REQUIRE(result == currency_to_get_history);
    server_thread.join();
}

TEST_CASE("get one rate history for currency") {
    ClientTest clientTest;
    Currency currency_to_get_history("currencyName", (33 << 0) + (3 << 8));
    std::vector<int8_t> expected_to_receive = {4, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 0, 0, 0, 0,
                                               '\\', 0};
    std::vector<int8_t> server_return = {33, 3, 0, 0, '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    auto result = client.getCurrencyWithHistory(currency_to_get_history);
    REQUIRE(result == currency_to_get_history);
    server_thread.join();
}

TEST_CASE("get two rates history for currency") {
    ClientTest clientTest;
    Currency currency_to_get_history("currencyName", {(33 << 0) + (3 << 8), 0});
    std::vector<int8_t> expected_to_receive = {4, 0, 0, 0,
                                               'c', 'u', 'r', 'r', 'e', 'n', 'c', 'y', 'N', 'a', 'm', 'e', 0, 0, 0, 0,
                                               '\\', 0};
    std::vector<int8_t> server_return = {33, 3, 0, 0, 0, 0, 0, 0, '\\', 0};
    std::thread server_thread(setup_server_returning, expected_to_receive, server_return, std::ref(clientTest));
    while (clientTest.PORTNO == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Client client = Client(clientTest.HOST, clientTest.PORTNO);
    auto result = client.getCurrencyWithHistory(currency_to_get_history);
    REQUIRE(result == currency_to_get_history);
    server_thread.join();
}