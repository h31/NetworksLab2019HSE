#pragma once

#include "pch.h"

namespace vacancy_service {

    struct VacancyInfo {
        VacancyInfo() = default;

        VacancyInfo(const VacancyInfo &) = default;

        VacancyInfo(VacancyInfo &&) = default;

        VacancyInfo &operator=(const VacancyInfo &) = default;

        VacancyInfo &operator=(VacancyInfo &&) = default;

        VacancyInfo(int32_t speciality_id, const std::string &company, const std::string &position,
                    int32_t min_age, int32_t max_age, int32_t salary);

        bool suits(int32_t speciality, int32_t age, int32_t salary) const;

        int32_t speciality_id;
        std::string company;
        std::string position;
        int32_t min_age;
        int32_t max_age;
        int32_t salary;
        bool available = true;
    };

    void to_json(nlohmann::json &j, const VacancyInfo &info);

    void from_json(const nlohmann::json &j, VacancyInfo &info);

} // namespace vacancy_service