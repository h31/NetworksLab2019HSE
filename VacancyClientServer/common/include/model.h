#ifndef VACANCY_COMMON_MODEL_H
#define VACANCY_COMMON_MODEL_H

#include <string>
#include "json.hpp"

namespace model {
    struct VacancyInfo {
        int32_t specialityId;
        std::string company;
        std::string position;
        int32_t minAge;
        int32_t maxAge;
        int32_t salary;

        VacancyInfo() = default;

        VacancyInfo(int32_t specialityId, const std::string &company, const std::string &position, int32_t minAge, int32_t maxAge,
                    int32_t salary);
    };

    void to_json(nlohmann::json& j, const VacancyInfo& info);
    void from_json(const nlohmann::json& j, VacancyInfo& info);

    struct SpecialityInfo {
        int32_t id;
        std::string speciality;

        SpecialityInfo() = default;
        SpecialityInfo(int32_t id, const std::string &speciality);
    };

    void to_json(nlohmann::json& j, const SpecialityInfo& info);
    void from_json(const nlohmann::json& j, SpecialityInfo& info);
}

#endif //VACANCY_COMMON_MODEL_H
