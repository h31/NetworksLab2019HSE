//
// Created by Владислав Калинин on 11/02/2019.
//

#include <fstream>
#include <zconf.h>
#include <iostream>
#include "../include/Tasks.h"
#include "../../common/include/Message.h"
#include "../include/utils.h"

void RegistryTask::operator()(int id) {
    std::string path = getWorkingPath() + "/users.txt";
    std::ifstream ifile(path);
    if (!ifile.is_open()) {
        ifile.close();
        std::ofstream ofs(path);
        ofs.close();
        (*this)(id);
    }
    std::string name, password;
    bool is_admin;
    while (!ifile.eof()) {
        ifile >> name >> password >> is_admin;
        if (this->name == name) {
            sendErrorResponse("username already exists");
            ifile.close();
            return;
        }
    }
    ifile.close();
    std::ofstream ofile(path, std::ios::app);
    ofile << this->name << "\t" << this->password << "\t" << 0 << std::endl;
    ofile.close();
    sendOkResponse();
}

ServerTask *ServerTask::create(const json &request, tbb::concurrent_unordered_map<int, User> *online_users, int fd) {
    int task_id = request.at("task_id").get<int>();
    switch (task_id) {
        case REGISTRY_TASK:
            return new RegistryTask(request, fd);
        case AUTHORIZATION_TASK:
            return new AuthorizationTask(request, online_users, fd);
        case BROADCAST_TASK:
            return new BroadcastTask(request, *online_users, fd);
        case PRIVATE_TASK:
            return new PrivateTask(request, *online_users, fd);
        case KICK_TASK:
            return new KickTask(request, online_users, fd);
        default:
            std::cout << "unknow command" << std::endl;
            break;
    }
    return nullptr;
}

void ServerTask::sendResponce(int socketfd, std::string msg) {
    try {
        std::string size = std::to_string(msg.size());
        write(socketfd, msg.c_str(), msg.size());
    } catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
}

void ServerTask::sendOkResponse() {
    std::string responce = ResponseMessage(STATUS_OK).to_json_format();
    sendResponce(fd, responce);
}

void ServerTask::sendErrorResponse(std::string msg) {
    std::string responce = ResponseMessage(STATUS_ERROR, msg).to_json_format();
    sendResponce(fd, responce);
}

void AuthorizationTask::operator()(int id) {
    std::string path = getWorkingPath() + "/users.txt";
    std::ifstream ifile(path);
    if (!ifile.is_open()) {
        ifile.close();
        std::ofstream ofs(path);
        ofs.close();
        (*this)(id);
    }
    std::string name, password;
    bool is_admin;
    while (!ifile.eof()) {
        ifile >> name >> password >> is_admin;
        if (this->name == name) {
            if (this->password == password) {
                this->online_users->insert({fd, User(name, is_admin)});
                sendOkResponse();
            } else {
                sendErrorResponse("wrong password");
            }
            ifile.close();
            return;
        }
    }
    ifile.close();
    sendErrorResponse("username not found");
}

void BroadcastTask::operator()(int id) {
    for (const auto &it : this->online_users) {
        try {
            if (it.first != fd) {
                sendResponce(it.first, msg);
            }
        } catch (std::exception e) {
            std::cout << e.what() << std::endl;
        }
    }
}

void PrivateTask::operator()(int id) {
    for (const auto &it : this->online_users) {
        try {
            if (it.second.name == receiver) {
                sendResponce(it.first, msg);
            }
        } catch (std::exception e) {
            std::cout << e.what() << std::endl;
        }
    }
}

void KickTask::operator()(int id) {
    User user = this->online_users->at(fd);
    if (!user.is_admin) {
        std::string msg = TextMessage("System", "not enough rights").to_json_format();
        sendResponce(fd, msg);
        return;
    }
    for (auto &it : *(this->online_users)) {
        try {
            if (it.second.name == name) {
                it.second.kick();
            }
        } catch (std::exception e) {
            std::cout << e.what() << std::endl;
        }
    }
}
