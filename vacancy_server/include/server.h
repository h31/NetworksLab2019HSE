#pragma once

#include "pch.h"
#include "speciality.h"
#include "vacancy.h"
#include "util/user_view.h"
#include "network/server_socket.h"

namespace vacancy_service {

    enum RequestStatus {
        OK = 0,
        ERROR = 1
    };

    struct Statistics {

        Statistics();

        std::atomic_uint32_t vacancy_count;
        std::atomic_uint32_t speciality_count;
        std::atomic_uint32_t client_count;
        tbb::concurrent_unordered_map<int32_t, SpecialityInfo> specialities;
        tbb::concurrent_unordered_map<int32_t, VacancyInfo> vacancies;
    };

    class Server {
    public:

        static const uint16_t DEFAULT_PORT = 1111;

        explicit Server(uint16_t port);

        void run();

        void shut_down();

    private:
        uint16_t port_;
        std::shared_ptr<Statistics> statistics_;
        network::ServerSocket socket_;
        bool is_alive_;

        void open_connection();

        enum RequestType {
            ADD_SPECIALITY = 1,
            ADD_VACANCY = 2,
            REMOVE_VACANCY = 3,
            GET_SPECIALITIES = 4,
            GET_VACANCIES = 5
        };

        class ClientHandler {
        public:
            ClientHandler(const std::shared_ptr<network::ClientSocket> &socket,
                          std::shared_ptr<Statistics> &statistics);

            void operator()();

        private:
            std::shared_ptr<network::ClientSocket> socket_;
            std::shared_ptr<Statistics> statistics_;

            nlohmann::json process_add_speciality_request(const nlohmann::json &request);

            nlohmann::json process_add_vacancy_request(const nlohmann::json &request);

            nlohmann::json process_remove_vacancy_request(const nlohmann::json &request);

            nlohmann::json process_get_specialities_request(const nlohmann::json &);

            nlohmann::json process_find_vacancies_request(const nlohmann::json &request);

            void process_request(const std::string &request);
        };

    };

} // namespace server