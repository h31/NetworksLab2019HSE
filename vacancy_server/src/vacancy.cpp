#include "vacancy.h"

namespace vacancy_service {

    VacancyInfo::VacancyInfo(int32_t speciality_id, const std::string &company, const std::string &position,
                             int32_t min_age, int32_t max_age, int32_t salary)
        : speciality_id(speciality_id),
          company(company),
          position(position),
          min_age(min_age),
          max_age(max_age),
          salary(salary) {}

    bool VacancyInfo::suits(int32_t speciality, int32_t age, int32_t salary) const {
        return available &&
               (speciality < 0 || speciality_id == speciality) &&
               (age < 0 || (min_age <= age && max_age >= age)) &&
               (salary < 0 || (this->salary < salary));
    }

    void to_json(nlohmann::json &j, const VacancyInfo &info) {
        j = {
            {SPECIALITY_ID, info.speciality_id},
            {COMPANY,       info.company},
            {POSITION,      info.position},
            {MIN_AGE,       info.min_age},
            {MAX_AGE,       info.max_age},
            {SALARY,        info.salary}
        };
    }

    void from_json(const nlohmann::json &j, VacancyInfo &info) {
        j.at(SPECIALITY_ID).get_to(info.speciality_id);
        j.at(COMPANY).get_to(info.company);
        j.at(POSITION).get_to(info.position);
        j.at(MIN_AGE).get_to(info.min_age);
        j.at(MAX_AGE).get_to(info.max_age);
        j.at(SALARY).get_to(info.salary);
    }
} // namespace vacancy_service