#include <email.h>

namespace email {

    Email::Email(const std::string &recipient, const std::string &author, const std::string &theme,
                 const std::string &body)
        : author_(author),
          recipient_(recipient),
          theme_(theme),
          body_(body) {}

    const std::string &Email::get_author() const {
        return author_;
    }

    const std::string &Email::get_recipient() const {
        return recipient_;
    }

    const std::string &Email::get_theme() const {
        return theme_;
    }

    const std::string &Email::get_body() const {
        return body_;
    }

    uint32_t Email::size() const {
        size_t result = 4 * INT_SIZE;
        result += recipient_.length();
        result += author_.length();
        result += theme_.length();
        result += body_.length();
        return static_cast<uint32_t>(result);
    }

    EmailInfo::EmailInfo(uint32_t id, const std::string &author, const std::string &theme)
        : id_(id),
          author_(author),
          theme_(theme) {}

    uint32_t EmailInfo::get_id() const {
        return id_;
    }

    const std::string &EmailInfo::get_author() const {
        return author_;
    }

    const std::string &EmailInfo::get_theme() const {
        return theme_;
    }

} // namespace email

