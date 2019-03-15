#pragma once

#include "pch.h"

namespace email {

    class Email {
    public:

        Email() = default;

        Email(const Email &other) = default;

        Email(const std::string &recipient, const std::string &author,
            const std::string &theme, const std::string &body);

        Email(Email&& email) noexcept = default;

        const std::string &get_author() const;

        const std::string &get_recipient() const;

        const std::string &get_theme() const;

        const std::string &get_body() const;

        friend std::ostream& operator<<(std::ostream& s, const Email &e);

    private:
        std::string author_;
        std::string recipient_;
        std::string theme_;
        std::string body_;
    };

    class EmailInfo {
    public:
        EmailInfo() = default;

        EmailInfo(uint32_t id, const std::string &author, const std::string &theme);

        uint32_t get_id() const;

        const std::string &get_author() const;

        const std::string &get_theme() const;

        friend std::ostream& operator<<(std::ostream& s, const EmailInfo &i);

    private:
        uint32_t id_;
        std::string author_;
        std::string theme_;
    };


}