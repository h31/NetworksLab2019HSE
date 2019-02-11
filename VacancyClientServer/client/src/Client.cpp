#include <Client.h>
#include <iostream>
#include <util.h>
#include <names.h>
#include <model.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

using namespace model;
using namespace nlohmann;
using namespace std;
using namespace vacancy;
using namespace util;

#define processBadResponse(response) if (response.at(SUCCESS).get<int32_t>()) { cout << "Error: " << response.at(CAUSE) << endl; return; }

Client::Client(const std::string &serverAddress, uint16_t port)
    : serverAddress(serverAddress)
    , port(port)
    , commands({
       {"add-speciality", make_shared<AddSpecialityCommand>()},
       {"add-vacancy", make_shared<AddVacancyCommand>()},
       {"remove-vacancy", make_shared<RemoveVacancyCommand>()},
       {"get-specialities", make_shared<GetSpecialitiesCommand>()},
       {"find-vacancy", make_shared<GetVacanciesCommand>()},
       {"help", make_shared<PrintHelpCommand>()}
    }) {}

int Client::openSocket() {
    struct sockaddr_in address;
    int sock = 0;
    ssize_t valread;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, serverAddress.c_str(), &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return sock;
}

void Client::run() {
    while (true) {
        cout << ":>  ";
        string command;
        getline(cin, command);
        if (command == "exit") {
            cout << "Good buy" << endl;
            break;
        }
        if (commands.find(command) == commands.end()) {
            command = "help";
        }
        Command *pCommand = commands[command].get();
        int socket;
        bool isHelpCommand = command == "help";
        if (isHelpCommand) {
            socket = -1;
        } else {
            socket = openSocket();
        }
        try {
            if (!isHelpCommand && socket < 0) throw "Network trouble";
            pCommand->execute(socket);
        } catch (const char * message) {
            cout << "Error: " << message << endl;
        } catch (const json::parse_error &exc) {
            cout << "Error: " << exc.what() << endl;
        };
        close(socket);
    }
}

void vacancy::Client::AddSpecialityCommand::execute(int socket) {
    string speciality;
    cout << "Enter speciality name :> ";
    getline(cin, speciality);
    json request;
    request[REQUEST_TYPE] = 1;
    request[SPECIALITY] = speciality;
    auto response = communicate(socket, request);
    processBadResponse(response);
    cout << "Speciality successfully added" << endl;
}

void Client::AddVacancyCommand::execute(int socket) {
    string company, position;
    cout << "Enter speciality id :> ";
    int32_t specialityId = readInt(false);
    cout << "Enter company name :> ";
    getline(cin, company);
    cout << "Enter position :> ";
    getline(cin, position);
    cout << "Enter minimum age :> ";
    int32_t minAge = readInt(false);
    cout << "Enter maximum age :> ";
    int32_t maxAge = readInt(false);
    cout << "Enter salary :> ";
    int32_t salary = readInt(false);
    VacancyInfo vacancyInfo(specialityId, company, position, minAge, maxAge, salary);

    json request;
    request[REQUEST_TYPE] = 2;
    request[VACANCY] = vacancyInfo;
    auto response = communicate(socket, request);
    processBadResponse(response);
    auto id = response.at(ID).get<int32_t>();
    cout << "Vacancy successfully added. Id: " << id << endl;
}

void Client::RemoveVacancyCommand::execute(int socket) {
    cout << "Enter vacancy id :> ";
    int32_t id = readInt(false);
    json request;
    request[REQUEST_TYPE] = 3;
    request[ID] = id;
    auto response = communicate(socket, request);
    processBadResponse(response);
    cout << "Vacancy successfully removed" << endl;
}

void Client::GetSpecialitiesCommand::execute(int socket) {
    json request;
    request[REQUEST_TYPE] = 4;
    auto response = communicate(socket, request);
    processBadResponse(response);
    json jsonInfos = response.at(SPECIALITIES);
    if (jsonInfos.empty()) {
        cout << "There is no specialities yet" << endl;
    } else {
        for (auto &jsonInfo : jsonInfos) {
            SpecialityInfo info = jsonInfo.get<SpecialityInfo>();
            cout << "Speciality: " << info.speciality << ". ID: " << info.id << endl << endl;
        }
    }
}

void Client::GetVacanciesCommand::execute(int socket) {
    cout << "Enter speciality id (or leave empty) :> ";
    int32_t specialityId = readInt(true);
    cout << "Enter age (or leave empty) :> ";
    int32_t age = readInt(true);
    cout << "Enter salary (or leave empty) :> ";
    int32_t salary = readInt(true);
    json request;
    request[SPECIALITY_ID] = specialityId;
    request[REQUEST_TYPE] = 5;
    request[AGE] = age;
    request[SALARY] = salary;
    auto response = communicate(socket, request);
    processBadResponse(response);
    json jsonInfos = response.at(VACANCIES);
    if (jsonInfos.empty()) {
        cout << "No vacancies found" << endl;
    } else {
        for (const auto &jsonInfo : jsonInfos) {
            VacancyInfo info = jsonInfo.get<VacancyInfo>();
            cout << "Company: " << info.company << endl;
            cout << "Position: " << info.position << endl;
            cout << "Minimum age: " << info.minAge << endl;
            cout << "Maximum age:" << info.maxAge << endl;
            cout << "Salary: " << info.salary << endl << endl;
        }
    }
}

int32_t vacancy::readInt(bool withDefault) {
    int32_t number = -1;
    std::string input;
    getline(cin, input);
    if (!input.empty()) {
        std::istringstream stream( input );
        stream >> number;
    }
    if (withDefault && number == -1) {
        return number;
    }
    if (number < 0) {
        throw "Number must be greater than zero";
    }
    return number;
}

void Client::PrintHelpCommand::execute(int socket) {
    cout << "Available commands:" << endl;
    cout << "  add-speciality" << endl;
    cout << "  add-vacancy" << endl;
    cout << "  remove-vacancy" << endl;
    cout << "  get-specialities" << endl;
    cout << "  find-vacancy" << endl;
    cout << "  help" << endl;
    cout << "  exit" << endl;
    cout << endl;
}
