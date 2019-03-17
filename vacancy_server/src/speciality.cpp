#include "speciality.h"

namespace vacancy_service {

    SpecialityInfo::SpecialityInfo(int32_t id, const std::string &speciality)
        : id(id),
          speciality(speciality) {}

    void to_json(nlohmann::json &json, const SpecialityInfo &info) {
        json = {
            {ID,         info.id},
            {SPECIALITY, info.speciality}
        };
    }

    void from_json(const nlohmann::json &json, SpecialityInfo &info) {
        json.at(ID).get_to(info.id);
        json.at(SPECIALITY).get_to(info.speciality);
    }

} // namespace vacancy_service