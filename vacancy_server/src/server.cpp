#include "server.h"

namespace vacancy_service {

    Statistics::Statistics()
        : vacancy_count(0),
          speciality_count(0),
          client_count(0),
          specialities(),
          vacancies() {}

    Server::Server(uint16_t port)
        : port_(port),
          statistics_(std::make_shared<Statistics>()),
          socket_(port_),
          is_alive_(true) {}

    void Server::run() {
        open_connection();
        while (is_alive_) {
            auto client = socket_.accept();
            if (client->is_alive()) {
                uint32_t id = statistics_->client_count++;
                util::UserView::println("Client " + std::to_string(id) + " connected");
                ClientHandler handler(client, statistics_);
                std::thread thread(handler);
                thread.detach();
            } else {
                util::UserView::println("Can't connect with the client!");
            }
        }
    }

    void Server::shut_down() {
        socket_.close_connection();
        is_alive_ = false;
    }

    void Server::open_connection() {
        try {
            socket_.open_connection();
        } catch (const network::socket_exception &exception) {
            util::UserView::println("Some have were acquired:");
            util::UserView::println(std::string(exception.what()));
        }
    }

    Server::ClientHandler::ClientHandler(const std::shared_ptr<network::ClientSocket> &socket,
                                         std::shared_ptr<Statistics> &statistics)
        : socket_(socket),
          statistics_(statistics) {}


    void Server::ClientHandler::operator()() {
        try {
            std::string request = socket_->receive_message();
            process_request(request);
        } catch (const network::socket_exception &exception) {
            util::UserView::println("Some errors have acquired:");
            util::UserView::println(std::string(exception.what()));
        }
        socket_->close();
    }

    void Server::ClientHandler::process_request(const std::string &message) {
        nlohmann::json response;

        try {
            nlohmann::json request = nlohmann::json::parse(message);
            int requestType = request.at(REQUEST_TYPE).get<int>();
            switch (requestType) {
                case ADD_SPECIALITY:
                    response = process_add_speciality_request(request);
                    break;
                case ADD_VACANCY:
                    response = process_add_vacancy_request(request);
                    break;
                case REMOVE_VACANCY:
                    response = process_remove_vacancy_request(request);
                    break;
                case GET_SPECIALITIES:
                    response = process_get_specialities_request(request);
                    break;
                case GET_VACANCIES:
                    response = process_find_vacancies_request(request);
                    break;
                default:
                    response[SUCCESS] = ERROR;
                    response[CAUSE] = "Unknown request type";
                    break;
            }
        } catch (const nlohmann::json::parse_error &exc) {
            response[SUCCESS] = ERROR;
            response[CAUSE] = "Error during parsing message";
        }
        socket_->send_message(response.dump());
    }

    nlohmann::json Server::ClientHandler::process_add_speciality_request(const nlohmann::json &request) {
        std::string speciality = request.at(SPECIALITY).get<std::string>();
        auto id = static_cast<int32_t>(statistics_->speciality_count++);
        statistics_->specialities[id] = SpecialityInfo(id, speciality);

        nlohmann::json response;
        response[SUCCESS] = OK;
        return response;
    }

    nlohmann::json Server::ClientHandler::process_add_vacancy_request(const nlohmann::json &request) {
        VacancyInfo vacancy = request.at(VACANCY).get<VacancyInfo>();
        auto id = static_cast<int32_t>(statistics_->vacancy_count++);
        statistics_->vacancies[id] = vacancy;

        nlohmann::json response;
        response[SUCCESS] = OK;
        response[ID] = id;
        return response;
    }

    nlohmann::json Server::ClientHandler::process_remove_vacancy_request(const nlohmann::json &request) {
        nlohmann::json response;
        auto id = request.at(ID).get<int32_t>();
        if (id >= static_cast<int32_t>(statistics_->vacancies.size())) {
            response[SUCCESS] = ERROR;
            response[CAUSE] = "Invalid vacancy id!";
            return response;
        }
        VacancyInfo &vacancy = statistics_->vacancies[id];
        vacancy.available = false;

        response[SUCCESS] = OK;
        return response;
    }

    nlohmann::json Server::ClientHandler::process_get_specialities_request(const nlohmann::json &) {
        std::vector<SpecialityInfo> infos;
        for (const auto &p : statistics_->specialities) {
            infos.push_back(p.second);
        }

        nlohmann::json response;
        response[SUCCESS] = OK;
        response[SPECIALITIES] = infos;
        return response;
    }

    nlohmann::json Server::ClientHandler::process_find_vacancies_request(const nlohmann::json &request) {
        auto speciality_id = request.at(SPECIALITY_ID).get<int32_t>();
        auto age = request.at(AGE).get<int32_t>();
        auto salary = request.at(SALARY).get<int32_t>();

        std::vector<VacancyInfo> infos;
        for (const auto &p : statistics_->vacancies) {
            const VacancyInfo &vacancy = p.second;
            if (vacancy.suits(speciality_id, age, salary)) {
                infos.push_back(vacancy);
            }
        }

        nlohmann::json response;
        response[SUCCESS] = OK;
        response[VACANCIES] = infos;
        return response;
    }

} // namespace vacancy