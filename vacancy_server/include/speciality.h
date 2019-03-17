#pragma once

#include "pch.h"

namespace vacancy_service {

    struct SpecialityInfo {
        SpecialityInfo() = default;

        SpecialityInfo(int32_t id, const std::string &speciality);

        int32_t id;
        std::string speciality;
    };

    void to_json(nlohmann::json &j, const SpecialityInfo &info);

    void from_json(const nlohmann::json &j, SpecialityInfo &info);

} // namespace vacancy_service