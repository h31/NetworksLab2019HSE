#ifndef VACANCY_SERVER_H
#define VACANCY_SERVER_H

#include <tbb/concurrent_unordered_map.h>
#include <stdint-gcc.h>
#include <atomic>
#include <model.h>
#include <json.hpp>
#include <set>

namespace vacancy {
    struct WrappedVacancyInfo {
        model::VacancyInfo info;
        bool removed;

        WrappedVacancyInfo() = default;
        explicit WrappedVacancyInfo(const model::VacancyInfo &info);
    };

    class Server {
    private:
        uint16_t port;
        std::atomic_uint32_t vacancyIdCounter;
        std::atomic_uint32_t specialityIdCounter;
        std::atomic_uint32_t clientIdCounter;
        tbb::concurrent_unordered_map<int32_t, model::SpecialityInfo> specialities;
        tbb::concurrent_unordered_map<int32_t, WrappedVacancyInfo> vacancies;
        std::set<int> sockets;
        std::atomic_bool onShutdown;

    public:
        explicit Server(uint16_t port);

        void runServer();

        void shutdown();

    private:
        class Worker {
        private:
            int socket;
            uint32_t clientId;
            Server &server;

        public:
            Worker(int socket, uint32_t clientId, Server &server);

            void operator() ();

        private:
            void processAddSpecialityRequest(const nlohmann::json &request);
            void processAddVacancyRequest(const nlohmann::json &request);
            void processRemoveVacancyRequest(const nlohmann::json &request);
            void processGetSpecialitiesRequest(const nlohmann::json &request);
            void processGetVacanciesRequest(const nlohmann::json &request);

            void sendResponse(const nlohmann::json &response);
        };
    };
}

#endif //VACANCY_SERVER_H
