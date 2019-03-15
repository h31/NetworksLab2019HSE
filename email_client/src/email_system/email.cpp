#include <email.h>

namespace email {

    Email::Email(const std::string &recipient, const std::string &author, const std::string &theme, const std::string &body)
                : author_(author)
                , recipient_(recipient)
                , theme_(theme)
                , body_(body)
                {}

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

    std::ostream &operator<<(std::ostream& s, const Email &e) {
        s << "author: " << e.get_author() << std::endl;
        s << "recipient: " << e.get_recipient() << std::endl;
        s << "theme: " << e.get_theme() << std::endl;
        s << "---------------------" << std::endl;
        s << e.get_body();
        return s;
    }

    EmailInfo::EmailInfo(uint32_t id, const std::string &author, const std::string &theme)
        : id_(id), author_(author), theme_(theme) {}

    uint32_t EmailInfo::get_id() const {
        return id_;
    }

    const std::string &EmailInfo::get_author() const {
        return author_;
    }

    const std::string &EmailInfo::get_theme() const {
        return theme_;
    }

    std::ostream &operator<<(std::ostream &s, const EmailInfo &i) {
        s << "id: " << i.get_id() << std::endl;
        s << "author: " << i.get_author() << std::endl;
        s << "theme: " << i.get_theme();
        return s;
    }
} // namespace email

