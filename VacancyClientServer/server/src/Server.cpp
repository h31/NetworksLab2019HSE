#include <Server.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <util.h>
#include <names.h>

using namespace nlohmann;
using namespace model;
using namespace util;
using namespace std;

#define PROCCESS_BAD_RESPONSE(response) catch (const json::parse_error &exc) { \
        response[SUCCESS] = 1; \
        response[CAUSE] = "parsing error"; \
    } catch (const char * msg) { \
        response[SUCCESS] = 1; \
        response[CAUSE] = msg; \
    }

vacancy::Server::Server(uint16_t port)
    : port(port)
    , vacancyIdCounter(0)
    , specialityIdCounter(0)
    , clientIdCounter(0)
    , specialities()
    , vacancies() {}

void vacancy::Server::runServer() {
    int serverFd, clientSocket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(serverFd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        try {
            if ((clientSocket = accept(serverFd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                continue;
            }
            uint32_t clientId = clientIdCounter++;
            cout << "Client " << clientId << " connected" << endl;
            Worker worker(clientSocket, clientId, *this);
            thread thr(worker);
            thr.detach();
        } catch (const char * msg) {
            cout << "Error: " << msg << endl;
        }
    }
#pragma clang diagnostic pop
}

vacancy::Server::Worker::Worker(int socket, uint32_t clientId, vacancy::Server &server)
    : socket(socket)
    , clientId(clientId)
    , server(server) {}


void vacancy::Server::Worker::operator()() {
    try {
        auto request = receiveMessage(socket);
        int requestType = request.at(REQUEST_TYPE).get<int>();
        switch (requestType) {
            case 1:
                processAddSpecialityRequest(request);
                break;
            case 2:
                processAddVacancyRequest(request);
                break;
            case 3:
                processRemoveVacancyRequest(request);
                break;
            case 4:
                processGetSpecialitiesRequest(request);
                break;
            case 5:
                processGetVacanciesRequest(request);
                break;
            default:
                cout << "Client: " << clientId << ". Unknown requestType: " << requestType << endl;
        }
    } catch (const char * msg) {
        cout << "Client: " << clientId << ". Error: " << msg << endl;
    }
    close(socket);
}

void vacancy::Server::Worker::processAddSpecialityRequest(const nlohmann::json &request) {
    json response;
    try {
        string speciality = request.at(SPECIALITY).get<string>();
        int32_t id = server.specialityIdCounter++;
        SpecialityInfo info(id, speciality);
        server.specialities[id] = info;
        response[SUCCESS] = 0;
    } PROCCESS_BAD_RESPONSE(response)
    sendResponse(response);
}

void vacancy::Server::Worker::processAddVacancyRequest(const nlohmann::json &request) {
    json response;
    try {
        WrappedVacancyInfo info(request.at(VACANCY).get<VacancyInfo>());
        int32_t id = server.vacancyIdCounter++;
        server.vacancies[id] = info;
        response[SUCCESS] = 0;
        response[ID] = id;
    } PROCCESS_BAD_RESPONSE(response)
    sendResponse(response);
}

void vacancy::Server::Worker::processRemoveVacancyRequest(const nlohmann::json &request) {
    json response;
    try {
        int32_t id = request.at(ID).get<int32_t>();
        if (id >= server.vacancies.size()) {
            throw "Vacancy was not founded";
        }
        WrappedVacancyInfo &info = server.vacancies[id];
        info.removed = true;
        response[SUCCESS] = 0;
    } PROCCESS_BAD_RESPONSE(response)
    sendResponse(response);
}

void vacancy::Server::Worker::processGetSpecialitiesRequest(const nlohmann::json &request) {
    json response;
    try {
        vector<SpecialityInfo> infos;
        for (const auto &entry : server.specialities) {
            auto &info = entry.second;
            infos.push_back(info);
        }
        response[SUCCESS] = 0;
        response[SPECIALITIES] = infos;
    } PROCCESS_BAD_RESPONSE(response)
    sendResponse(response);
}

void vacancy::Server::Worker::processGetVacanciesRequest(const nlohmann::json &request) {
    json response;
    try {
        int32_t specialityId = request.at(SPECIALITY_ID).get<int32_t>();
        int32_t age = request.at(AGE).get<int32_t>();
        int32_t salary = request.at(SALARY).get<int32_t>();
        vector<VacancyInfo> infos;
        for (const auto &entry : server.vacancies) {
            if (entry.second.removed) continue;
            const VacancyInfo &info = entry.second.info;
            if (specialityId > 0 && info.specialityId != specialityId) continue;
            if (age > 0 && !(info.minAge <= age && info.maxAge >= age)) continue;
            if (salary > 0 && info.salary < salary) continue;
            infos.push_back(info);
        }
        response[SUCCESS] = 0;
        response[VACANCIES] = infos;
    } PROCCESS_BAD_RESPONSE(response)
    sendResponse(response);
}

void vacancy::Server::Worker::sendResponse(const nlohmann::json &response) {
    sendMessage(socket, response);
}

vacancy::WrappedVacancyInfo::WrappedVacancyInfo(const model::VacancyInfo &info)
    : info(info)
    , removed(false) {}
