#include "../include/BugTrackingServer.h"
#include <thread>
#include <netinet/in.h>
#include <strings.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>

BugTrackingServer::BugTrackingServer(UserService* userService) : _userService(userService) {}

void BugTrackingServer::initServer(uint16_t port_number) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    sockaddr_in server_addr = {0}, cli_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    listen(socket_fd, 5);

    while (true) {
        unsigned int client = sizeof(cli_addr);
        int new_socket_fd = accept(socket_fd, (struct sockaddr*) &cli_addr, &client);
        if (new_socket_fd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        std::thread thread(&BugTrackingServer::process_client, this, new_socket_fd);
        thread.detach();
    }
}

void BugTrackingServer::process_client(int sock_fd) {
    std::cout << "New client connected on " << sock_fd << ".\n";
    Client client = Client(sock_fd, _userService);
    _clients_mutex.lock();
    _clients[sock_fd] = &client;
    _clients_mutex.unlock();
    uint32_t code;
    while (true) {
        if (!readInt32(sock_fd, code)) {
            std::cout << "Failed to process message from client " << sock_fd << ". Closing connection.\n";
            close(client);
            break;
        } else {
            if (code == 100) {
                authorize(client);
            } else if (code == 200) {
                bugsTesterList(client);
            } else if (code == 300) {
                bugVerification(client);
            } else if (code == 400) {
                bugsDeveloperList(client);
            } else if (code == 500) {
                bugFix(client);
            } else if (code == 600) {
                bugRegister(client);
            } else if (code == 700) {
                close(client);
                break;
            } else {
                std::cout << "Client with id " << sock_fd << " has sent unknown request ";
                writeInt32(sock_fd, 1);
            }
        }
    }
}

bool BugTrackingServer::authorize(BugTrackingServer::Client& client) {
    int sock_fd = client.sock_fd;
    uint32_t id;
    readInt32(sock_fd, id);
    if (client.isAuthorized()) {
        std::cout << "Client on socket " << sock_fd << " is already authorized with id " << client.user.id;
        writeInt32(sock_fd, 101);
        writeInt32(sock_fd, static_cast<uint32_t>(client.user.id));
        writeInt32(sock_fd, client.user.role);
        return false;
    }
    if (client.authorize(id)) {
        writeInt32(sock_fd, 101);
        writeInt32(sock_fd, client.user.role);
        std::cout << "Client on socket " << sock_fd << " has authorized with id " << client.user.id;
        std::cout << " and role " << client.user.role << "\n";
        return true;
    } else {
        writeInt32(sock_fd, 102);
        writeInt32(sock_fd, id);
        std::cout << "Client on socket " << sock_fd << " has failed authorization\n";
        return false;
    }
}

bool BugTrackingServer::bugsTesterList(BugTrackingServer::Client &client) {
    int sock_fd = client.sock_fd;
    uint32_t bugStatus;
    readInt32(sock_fd, bugStatus);
    if (!client.isAuthorized()) {
        std::cout << "Request from unauthorized client " << client.sock_fd << "\n";
        writeInt32(sock_fd, 2);
        return false;
    }
    if (client.user.role != UserService::User::Role::TESTER) {
        std::cout << "User with id " << client.user.id << " is not tester and cannot see bugs\n";
        writeInt32(sock_fd, 3);
        writeInt32(sock_fd, UserService::User::Role::TESTER);
        return false;
    }
    std::vector<Bug> bugsList;
    _bugs_mutex.lock();
    for (const auto &entry : _bugs) {
        if (entry.second.developer_id == client.user.id && entry.second.status == bugStatus) {
            bugsList.push_back(entry.second);
        }
    }
    _bugs_mutex.unlock();
    std::cout << "Tester with id " << client.user.id << " requested bugs with status " << bugStatus << "\n";
    writeInt32(sock_fd, static_cast<uint32_t>(bugsList.size()));
    for (const auto &bug : bugsList) {
        writeInt32(sock_fd, bug.id);
        writeString(sock_fd, bug.description);
    }
    return true;
}

bool BugTrackingServer::bugVerification(BugTrackingServer::Client &client) {
    int sock_fd = client.sock_fd;
    uint32_t bugId, verificationCode;
    readInt32(sock_fd, bugId);
    readInt32(sock_fd, verificationCode);
    if (!client.isAuthorized()) {
        std::cout << "Request from unauthorized client " << client.sock_fd << "\n";
        writeInt32(sock_fd, 2);
        return false;
    }
    if (client.user.role != UserService::User::Role::TESTER) {
        std::cout << "Client with id " << client.user.id << " is not tester and cannot verify bug: " << bugId << "\n";
        writeInt32(sock_fd, 3);
        writeInt32(sock_fd, UserService::User::Role::TESTER);
        return false;
    }
    _bugs_mutex.lock();
    if (_bugs.find(bugId) == _bugs.end()) {
        _bugs_mutex.unlock();
        std::cout << "Client with id " << client.user.id << " tried to verify non existing bug with id " << bugId << "\n";
        writeInt32(sock_fd, 303);
        writeInt32(sock_fd, bugId);
        return false;
    } else {
        if (_bugs[bugId].status == Bug::BugStatus::NEW) {
            _bugs_mutex.unlock();
            std::cout << "Tester with id " << client.user.id << " tried to verify not fixed bug with id " << bugId << "\n";
            writeInt32(sock_fd, 304);
            writeInt32(sock_fd, bugId);
            return false;
        } else if (_bugs[bugId].status == Bug::BugStatus::QA){
            if (verificationCode == 0) {
                _bugs[bugId].status = Bug::BugStatus::NEW;
                _bugs_mutex.unlock();
                std::cout << "Tester with id " << client.user.id << " has rejected bug fix with id " << bugId << "\n";
            } else if (verificationCode == 1) {
                _bugs[bugId].status = Bug::BugStatus::FIXED;
                _bugs_mutex.unlock();
                std::cout << "Tester with id " << client.user.id << " has approved bug fix with id " << bugId << "\n";
            } else {
                _bugs_mutex.unlock();
                std::cout << "Tester with id " << client.user.id << " has sent illegal verification code " << verificationCode << "\n";
                writeInt32(sock_fd, 304);
                writeInt32(sock_fd, verificationCode);
                return false;
            }
            writeInt32(sock_fd, 301);
            writeInt32(sock_fd, bugId);
            writeInt32(sock_fd, verificationCode);
            return true;
        } else {
            std::cout << "Tester with id " << client.user.id << " tried to verify closed bug with id " << bugId << "\n";
            writeInt32(sock_fd, 302);
            writeInt32(sock_fd, bugId);
            return false;
        }
    }
}

bool BugTrackingServer::bugsDeveloperList(BugTrackingServer::Client &client) {
    int sock_fd = client.sock_fd;
    if (!client.isAuthorized()) {
        std::cout << "Request from unauthorized client " << client.sock_fd << "\n";
        writeInt32(sock_fd, 2);
        return false;
    }
    if (client.user.role != UserService::User::Role::DEVELOPER) {
        std::cout << "User with id " << client.user.id << " is not developer and does not have bugs\n";
        writeInt32(sock_fd, 3);
        writeInt32(sock_fd, UserService::User::Role::DEVELOPER);
        return false;
    }
    std::vector<Bug> clientBugs;
    _bugs_mutex.lock();
    for (const auto &entry : _bugs) {
        if (entry.second.developer_id == client.user.id && entry.second.status == Bug::BugStatus::NEW) {
            clientBugs.push_back(entry.second);
        }
    }
    _bugs_mutex.unlock();
    std::cout << "Developer with id " << client.user.id << " requested his bug-list\n";
    writeInt32(sock_fd, static_cast<uint32_t>(clientBugs.size()));
    for (const auto &bug : clientBugs) {
        writeInt32(sock_fd, bug.id);
        writeString(sock_fd, bug.description);
    }
    return true;
}

bool BugTrackingServer::bugFix(BugTrackingServer::Client &client) {
    int sock_fd = client.sock_fd;
    uint32_t bugId;
    readInt32(sock_fd, bugId);
    if (!client.isAuthorized()) {
        std::cout << "Request from unauthorized client " << client.sock_fd << "\n";
        writeInt32(sock_fd, 2);
        return false;
    }
    if (client.user.role != UserService::User::Role::DEVELOPER) {
        std::cout << "User with id " << client.user.id << " is not developer and cannot fix bug: " << bugId << "\n";
        writeInt32(sock_fd, 3);
        writeInt32(sock_fd, UserService::User::Role::DEVELOPER);
        return false;
    }
    _bugs_mutex.lock();
    if (_bugs.find(bugId) == _bugs.end()) {
        _bugs_mutex.unlock();
        std::cout << "Developer with id " << client.user.id << " tried to fix non existing bug with id " << bugId << "\n";
        writeInt32(sock_fd, 503);
        writeInt32(sock_fd, bugId);
        return false;
    } else {
        if (_bugs[bugId].status != Bug::BugStatus::NEW) {
            Bug::BugStatus status = _bugs[bugId].status;
            _bugs_mutex.unlock();
            std::cout << "Developer with id " << client.user.id << " tried to fix not actual bug with id " << bugId << "\n";
            writeInt32(sock_fd, 502);
            writeInt32(sock_fd, status);
            return false;
        } else {
            _bugs[bugId].status = Bug::BugStatus::QA;
            _bugs_mutex.unlock();
            std::cout << "Developer with id " << client.user.id << " has fixed bug with id " << bugId << "\n";
            writeInt32(sock_fd, 501);
            writeInt32(sock_fd, bugId);
            return true;
        }
    }
}

bool BugTrackingServer::bugRegister(BugTrackingServer::Client &client) {
    int sock_fd = client.sock_fd;
    std::string description;
    Bug bug;
    readInt32(sock_fd, bug.id);
    readInt32(sock_fd, bug.developer_id);
    readInt32(sock_fd, bug.project_id);
    readString(sock_fd, bug.description);
    if (!client.isAuthorized()) {
        std::cout << "Request from unauthorized client " << client.sock_fd << "\n";
        writeInt32(sock_fd, 2);
        return false;
    }
    if (client.user.role != UserService::User::Role::TESTER) {
        std::cout << "User with id " << client.user.id << " is not tester and cannot register bugs" << bug.id << "\n";
        writeInt32(sock_fd, 3);
        writeInt32(sock_fd, UserService::User::Role::TESTER);
        return false;
    }
    _bugs_mutex.lock();
    if (_bugs.find(bug.id) != _bugs.end()) {
        _bugs_mutex.unlock();
        std::cout << "Tester with id " << client.user.id << " tried to register existing bug with id " << bug.id << "\n";
        writeInt32(sock_fd, 602);
        writeInt32(sock_fd, bug.id);
        return false;
    } else {
        _bugs[bug.id] = bug;
        _bugs_mutex.unlock();
        writeInt32(sock_fd, 601);
        writeInt32(sock_fd, bug.id);
        std::cout << "Tester with id " << client.user.id << " has registered new bug with id " << bug.id << '\n';
        return true;
    }
}

bool BugTrackingServer::close(BugTrackingServer::Client &client) {
    _clients_mutex.lock();
    if (_clients.find(client.sock_fd) == _clients.end()) {
        _clients_mutex.unlock();
        std::cout << "Client with socket " << client.sock_fd << " is already disconnected\n";
        return false;
    } else {
        _clients.erase(client.sock_fd);
        _clients_mutex.unlock();
        std::cout << "Client with socket " << client.sock_fd << " has been disconnected\n";
        return true;
    }
}

bool BugTrackingServer::banClient(int id) {
    _clients_mutex.lock();
    if (_clients.find(id) == _clients.end()) {
        _clients_mutex.unlock();
        std::cout << "Client with id " << id << " was not found\n";
        return false;
    } else {
        Client* client = _clients[id];
        _clients_mutex.unlock();
        std::cout << "Client with id " << id << " has been banned\n";
        return close(*client);
    }
}

/* READING FUNCTIONS */

bool BugTrackingServer::readInt32(int sock_fd, uint32_t& dst) {
    ssize_t n = read(sock_fd, &dst, sizeof(uint32_t));
    if (n < 0) {
        return false;
    }
    dst = ntohl(dst);
    return true;
}

bool BugTrackingServer::readString(int sock_fd, std::string& dst) {
    uint32_t length;
    if (!readInt32(sock_fd, length)) {
        return false;
    }
    char buf[length + 1];
    buf[length] = '\0';
    ssize_t n = read(sock_fd, buf, length);
    if (n < 0) {
        return false;
    }
    dst = std::string(buf);
    return true;
}

/* WRITING FUNCTIONS */

bool BugTrackingServer::writeInt32(int sock_fd, uint32_t number) {
    uint32_t value = htonl(number);
    return ::write(sock_fd, &value, sizeof(uint32_t)) >= 0;
}

bool BugTrackingServer::writeString(int sock_fd, std::string data) {
    auto length = static_cast<uint32_t>(data.length());
    if (!writeInt32(sock_fd, length)) {
        return false;
    }
    ssize_t written = ::write(sock_fd, data.c_str(), length);
    return written >= 0;
}

/*  CLIENT FUNCTIONS */

BugTrackingServer::Client::Client(
        int socket_fd,
        UserService* userService
) : sock_fd(socket_fd),  _userService(userService) {}

bool BugTrackingServer::Client::isAuthorized() {
    return user.isEmpty();
}

bool BugTrackingServer::Client::authorize(int id) {
    UserService::User newUser = _userService->getUser(id);
    if (newUser.isEmpty()) {
        return false;
    }
    user = newUser;
    return true;
}