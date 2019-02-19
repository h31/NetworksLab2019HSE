#include <iostream>
#include <strings.h>
#include "client.h"

client::client(const char *host, uint16_t port_number) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    hostent* server = gethostbyname(host);

    if (sockfd < 0) {
        std::cerr << "ERROR opening socket\n";
        exit(1);
    }

    if (server == nullptr) {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }

    sockaddr_in serv_addr{};
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port_number);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting\n";
        exit(1);
    }
    std::cout << "Connection established\n";
}

void client::registerUser() {
    int usr_id;
    std::cin >> usr_id;
    sendNum(REGISTER_USER);
    sendNum(usr_id);
    int answer_type = getNum();
    switch (answer_type) {
        case REGISTER_USER_SUCC: {
            int role = getNum();
            std::cout << "Register user with id: " + std::to_string(role) + '\n';
            break;
        }
        case REGISTER_USER_UNDEFINED_ID: {
            int type = getNum();
            std::cout << "Can't register user id " + std::to_string(type) + '\n';
            break;
        }
        case REGISTER_USER_AUTORIZE: {
            int user_id = getNum();
            int user_role = getNum();
            std::cout << "User with id " << user_id << " and role " << user_role << " already authorized\n";
            break;
        }
        default: {
            std::cout << "Undefined server code\n";
        }
    }
}

int client::getNum() {
    int num;
    int n = read(sockfd, &num, sizeof(int32_t));
    if (n < 0) {
        terminate();
    }
    return ntohl(num);
}

void client::sendNum(int num) {
    num = htonl(num);
    write(sockfd, &num, sizeof(int32_t));
}

void client::sendString(std::string string_to_send) {
    int length = string_to_send.length();
    sendNum(length);
    write(sockfd, string_to_send.c_str(), length);
}

std::string client::getString() {
    int length = getNum();
    char* buf = new char[length + 1];
    int n = read(sockfd, buf, length);
    buf[length] = '\0';
    std::string ans(buf);
    delete[] buf;
    if (n < 0) {
        terminate();
    }
    return ans;
}

void client::terminate() {
    std::cout << "Program end\n";
    exit(0);
}

void client::getBugsTester() {
    std::string bug_type;
    bug_status bug_code = UNDEFINED;
    std::cin >> bug_type;
    if (bug_type == "active") {
        bug_code = ACTIVE;
    }
    if (bug_type == "close") {
        bug_code = CLOSE;
    }
    if (bug_code == UNDEFINED) {
        std::cout << "Undefined type of bug: " + bug_type;
    }
    sendNum(GET_TESTER_BUG_LIST);
    sendNum(bug_code);
    int type = getNum();
    switch (type) {
        case GET_TESTER_BUG_LIST_SUCC: {
            int bug_count = getNum();
            for (int i = 0; i < bug_count; ++i) {
                int bug_id = getNum();
                int project_id = getNum();
                std::string description = getString();
                std::cout << "Bug with id: " << bug_id << "project id: " << project_id << " description: " << description;
            }
            break;
        }
        case UNKNOWN_REQUEST: {
            int ver_code = getNum();
            std::cout << "Unknown request code " << ver_code << '\n';
            break;
        }
        case UNAUTORIZE: {
            std::cout << "You not authorize\n";
            break;
        }
        case CHECK_RIGHTS: {
            std::cout << "You don't have rights\n";
            break;
        }
        default: {
            std::cout << "Undefined server code\n";
        }
    }
}

void client::confirmBugTester() {
    int bug_id;
    std::cin >> bug_id;
    sendNum(PROCESS_BUG_TESTER);
    sendNum(bug_id);
    sendNum(ACCEPT);
    int type = getNum();
    switch (type) {
        case PROCESS_BUG_TESTER_SUCC: {
            bug_id = getNum();
            std::cout << "Accept bug with id: " << bug_id << '\n';
            break;
        }
        case PROCESS_BUG_TESTER_NON_ACTIVE_BUG: {
            int bug_status = getNum();
            std::cout << "With bug was close with status " << bug_status << '\n';
            break;
        }
        case PROCESS_BUG_TESTER_NO_BUG_ID: {
            bug_id = getNum();
            std::cout << "No bug with id: " << bug_id << '\n';
            break;
        }
        case PROCESS_BUG_TESTER_NOT_FIX: {
            bug_id = getNum();
            std::cout << "Bug with id: " << bug_id << ' not fix' << '\n';
            break;
        }
        case PROCESS_BUG_TESTER_UNKNOWN_CODE: {
            int ver_code = getNum();
            std::cout << "Unknown verify code " << ver_code << '\n';
            break;
        }
        case UNKNOWN_REQUEST: {
            int ver_code = getNum();
            std::cout << "Unknown request code " << ver_code << '\n';
            break;
        }
        case UNAUTORIZE: {
            std::cout << "You not authorize\n";
            break;
        }
        case CHECK_RIGHTS: {
            std::cout << "You don't have rights\n";
            break;
        }
        default: {
            std::cout << "Undefined server code\n";
        }
    }
}

void client::rejectBugTester() {
    int bug_id;
    std::cin >> bug_id;
    sendNum(PROCESS_BUG_TESTER);
    sendNum(bug_id);
    sendNum(DECLINE);
    int type = getNum();
    switch (type) {
        case PROCESS_BUG_TESTER_SUCC: {
            bug_id = getNum();
            std::cout << "Accept bug with id: " << bug_id << '\n';
            break;
        }
        case PROCESS_BUG_TESTER_NON_ACTIVE_BUG: {
            int bug_status = getNum();
            std::cout << "With bug was close with status " << bug_status << '\n';
            break;
        }
        case PROCESS_BUG_TESTER_NO_BUG_ID: {
            bug_id = getNum();
            std::cout << "No bug with id: " << bug_id << '\n';
            break;
        }
        case PROCESS_BUG_TESTER_NOT_FIX: {
            bug_id = getNum();
            std::cout << "Bug with id: " << bug_id << ' not fix' << '\n';
            break;
        }
        case PROCESS_BUG_TESTER_UNKNOWN_CODE: {
            int ver_code = getNum();
            std::cout << "Unknown verify code " << ver_code << '\n';
            break;
        }
        case UNKNOWN_REQUEST: {
            int ver_code = getNum();
            std::cout << "Unknown request code " << ver_code << '\n';
            break;
        }
        case UNAUTORIZE: {
            std::cout << "You not authorize\n";
            break;
        }
        case CHECK_RIGHTS: {
            std::cout << "You don't have rights\n";
            break;
        }
        default: {
            std::cout << "Undefined server code\n";
        }
    }
}

void client::getBugsDeveloper() {
    sendNum(GET_DEVELOPER_BUGS);
    int type = getNum();
    switch (type) {
        case GET_DEVELOPER_BUGS_SUCC: {
            int bugs_count = getNum();
            for (int i = 0; i < bugs_count; ++i) {
                int bug_id = getNum();
                std::string description = getString();
                std::cout << "Bug with id: " << bug_id << " with description: " << description << '\n';
            }
            break;
        }
        case GET_DEVELOPER_BUGS_NO_ID: {
            int developer_id = getNum();
            std::cout << "No developer with id: " << developer_id << '\n';
            break;
        }
        case UNKNOWN_REQUEST: {
            int ver_code = getNum();
            std::cout << "Unknown request code " << ver_code << '\n';
            break;
        }
        case UNAUTORIZE: {
            std::cout << "You not authorize\n";
            break;
        }
        case CHECK_RIGHTS: {
            std::cout << "You don't have rights\n";
            break;
        }
        default: {
            std::cout << "Undefined server code\n";
            break;
        }
    }
}

void client::closeBugDeveloper() {
    int bud_id;
    std::cin >> bud_id;
    sendNum(CLOSE_BUG_DEVELOPER);
    sendNum(bud_id);
    int type = getNum();
    switch (type) {
        case CLOSE_BUG_DEVELOPER_SUCC: {
            bud_id = getNum();
            std::cout << "Clode bug with id: " << bud_id << '\n';
            break;
        }
        case CLOSE_BUG_DEVELOPER_NO_ID: {
            bud_id = getNum();
            std::cout << "No bug with id: " << bud_id << '\n';
            break;
        }
        case CLOSE_BUG_DEVELOPER_ALREADY_CLOSE: {
            int bug_status = getNum();
            std::cout << "Bug already close : " << bug_status << '\n';
            break;
        }
        case UNKNOWN_REQUEST: {
            int ver_code = getNum();
            std::cout << "Unknown request code " << ver_code << '\n';
            break;
        }
        case UNAUTORIZE: {
            std::cout << "You not authorize\n";
            break;
        }
        case CHECK_RIGHTS: {
            std::cout << "You don't have rights\n";
            break;
        }
        default: {
            std::cout << "Undefined server code\n";
            break;
        }
    }
}

void client::addNewBug() {
    int developer_id = 0, project_id = 0, bug_id = 0;
    std::string description;
    std::cout << bug_id <<  developer_id << project_id << description;
    sendNum(SEND_NEW_BUG);
    sendNum(bug_id);
    sendNum(developer_id);
    sendNum(project_id);
    sendString(description);
    int type = getNum();
    switch (type) {
        case SEND_NEW_BUG_SUCC: {
            int bug_id = getNum();
            std::cout << "Create new bug with id: " << bug_id << '\n';
            break;
        }
        case SEND_NEW_BUG_EXIST: {
            int bug_id = getNum();
            std::cout << "Bud with id: " << bug_id << "exist \n";
            break;
        }
        case UNKNOWN_REQUEST: {
            int ver_code = getNum();
            std::cout << "Unknown request code " << ver_code << '\n';
            break;
        }
        case UNAUTORIZE: {
            std::cout << "You not authorize\n";
            break;
        }
        case CHECK_RIGHTS: {
            std::cout << "You don't have rights\n";
            break;
        }
        default:break;
    }
}

void client::quit() {
    sendNum(CLIENT_TERMINATE);
    terminate();
}
