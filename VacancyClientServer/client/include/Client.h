#ifndef VACANCY_CLIENT_H
#define VACANCY_CLIENT_H

#include <memory>
#include <string>
#include <unordered_map>
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
        };


        class AddVacancyCommand : public Command {
        public:
            void execute(int socket) override;
        };


        class RemoveVacancyCommand : public Command {
        public:
            void execute(int socket) override;
        };


        class GetSpecialitiesCommand : public Command {
        public:
            void execute(int socket) override;
        };


        class GetVacanciesCommand : public Command {
        public:
            void execute(int socket) override;
        };


        class PrintHelpCommand : public Command {
            void execute(int socket) override;
        };
    };

    int32_t readInt(bool withDefault);
}
#endif //VACANCY_CLIENT_H
