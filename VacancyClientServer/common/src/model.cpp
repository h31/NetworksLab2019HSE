#include <model.h>
#include "names.h"

using namespace model;
using namespace nlohmann;

VacancyInfo::VacancyInfo(int32_t specialityId, const std::string &company, const std::string &position, int32_t minAge, int32_t maxAge,
                         int32_t salary)
        : company(company)
        , position(position)
        , minAge(minAge)
        , maxAge(maxAge)
        , salary(salary) {}

SpecialityInfo::SpecialityInfo(int32_t id, const std::string &speciality)
        : id(id)
        , speciality(speciality) {}

void model::to_json(json &j, const VacancyInfo &info) {
    j = json({
        {SPECIALITY_ID, info.specialityId},
        {COMPANY, info.company},
        {POSITION, info.position},
        {MIN_AGE, info.minAge},
        {MAX_AGE, info.maxAge},
        {SALARY, info.salary}
        });
}

void model::from_json(const json &j, VacancyInfo &info) {
    j.at(SPECIALITY_ID).get_to(info.specialityId);
    j.at(COMPANY).get_to(info.company);
    j.at(POSITION).get_to(info.position);
    j.at(MIN_AGE).get_to(info.minAge);
    j.at(MAX_AGE).get_to(info.maxAge);
    j.at(SALARY).get_to(info.salary);
}

void model::to_json(nlohmann::json &j, const SpecialityInfo &info) {
    j = json({{ID, info.id}, {SPECIALITY, info.speciality}});
}

void model::from_json(const nlohmann::json &j, SpecialityInfo &info) {
    j.at(ID).get_to(info.id);
    j.at(SPECIALITY).get_to(info.speciality);
}
