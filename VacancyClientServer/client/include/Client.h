#ifndef VACANCY_CLIENT_H
#define VACANCY_CLIENT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <model.h>
#include "../../common/include/json.hpp"

namespace vacancy {
    class Command {
    public:
        virtual void execute(int socket) = 0;

        virtual ~Command() = default;
    };


    class Client {
    private:
        const std::string serverAddress;
        const uint16_t port;
        std::unordered_map<std::string, std::shared_ptr<Command>> commands;
    public:
        Client(const std::string &serverAddress, uint16_t port);

        void run();

    private:
        int openSocket();

        class AddSpecialityCommand : public Command {
        public:
            void execute(int socket) override;

        private:
            nlohmann::json sendRequest(int socket, const std::string &speciality) const;
        };


        class AddVacancyCommand : public Command {
        public:
            void execute(int socket) override;

        private:
            nlohmann::json sendRequest(int socket, const model::VacancyInfo &vacancyInfo) const;
        };


        class RemoveVacancyCommand : public Command {
        public:
            void execute(int socket) override;

        private:
            nlohmann::json sendRequest(int socket, int32_t id) const;
        };


        class GetSpecialitiesCommand : public Command {
        public:
            void execute(int socket) override;

        private:
            nlohmann::json sendRequest(int socket) const;
        };


        class GetVacanciesCommand : public Command {
        public:
            void execute(int socket) override;

        private:
            nlohmann::json sendRequest(int socket, int32_t specialityId, int32_t age, int32_t salary) const;
        };


        class PrintHelpCommand : public Command {
            void execute(int socket) override;
        };
    };

    int32_t readInt(bool withDefault);
}
#endif //VACANCY_CLIENT_H
