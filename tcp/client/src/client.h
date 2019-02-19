#ifndef CLIENT_TESTER_H
#define CLIENT_TESTER_H

#include <cstdint>
#include <string>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

enum bug_status {ACTIVE = 0, CLOSE = 1, UNDEFINED = 3};
enum verification_code {DECLINE = 0, ACCEPT = 1};
enum message_code {
    REGISTER_USER = 100,
    REGISTER_USER_SUCC = 101,
    REGISTER_USER_UNDEFINED_ID = 102,
    REGISTER_USER_AUTORIZE = 103,
    GET_TESTER_BUG_LIST = 200,
    GET_TESTER_BUG_LIST_SUCC = 201,
    PROCESS_BUG_TESTER = 300,
    PROCESS_BUG_TESTER_SUCC = 301,
    PROCESS_BUG_TESTER_NON_ACTIVE_BUG = 302,
    PROCESS_BUG_TESTER_NO_BUG_ID = 303,
    PROCESS_BUG_TESTER_NOT_FIX = 304,
    PROCESS_BUG_TESTER_UNKNOWN_CODE = 305,
    GET_DEVELOPER_BUGS = 400,
    GET_DEVELOPER_BUGS_SUCC = 401,
    GET_DEVELOPER_BUGS_NO_ID = 402,
    CLOSE_BUG_DEVELOPER = 500,
    CLOSE_BUG_DEVELOPER_SUCC = 501,
    CLOSE_BUG_DEVELOPER_NO_ID = 502,
    CLOSE_BUG_DEVELOPER_ALREADY_CLOSE = 503,
    SEND_NEW_BUG = 600,
    SEND_NEW_BUG_SUCC = 601,
    SEND_NEW_BUG_EXIST = 602,
    CLIENT_TERMINATE = 700,
    UNKNOWN_REQUEST = 1,
    UNAUTORIZE = 2,
    CHECK_RIGHTS = 3,
};

class client {
    public:
        client(const char* host, uint16_t port_number);
        void registerUser();
        void getBugsTester();
        void confirmBugTester();
        void rejectBugTester();
        void getBugsDeveloper();
        void closeBugDeveloper();
        void addNewBug();
        void quit();

    private:
        int sockfd;
        int getNum();
        void sendNum(int num);
        void sendString(std::string string_to_send);
        std::string getString();
        void terminate();
};


#endif //CLIENT_TESTER_H
