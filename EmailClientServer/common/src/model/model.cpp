
#include <model/model.h>

#include "model/model.h"
#include "../../../../VacancyClientServer/common/include/model.h"

using namespace model;

Email::Email(const std::string &theme, const std::string &body, const std::string &author, const std::string &recipient)
        : theme(theme), body(body), author(author), recipient(recipient) {}

const std::string &Email::getTheme() const {
    return theme;
}

const std::string &Email::getBody() const {
    return body;
}

const std::string &Email::getAuthor() const {
    return author;
}

const std::string &Email::getRecipient() const {
    return recipient;
}

bool Email::operator==(const Email &other) const {
    return (theme == other.theme) && (body == other.body) && (author == other.author) && (recipient == other.recipient);
}

EmailInfo::EmailInfo(uint32_t id, const std::string &author, const std::string &theme) : id(id), author(author),
                                                                                         theme(theme) {}

uint32_t EmailInfo::getId() const {
    return id;
}

const std::string &EmailInfo::getAuthor() const {
    return author;
}

const std::string &EmailInfo::getTheme() const {
    return theme;
}

bool EmailInfo::operator==(const EmailInfo &other) const {
    return (id == other.id) && (author == other.author) && (theme == other.theme);
}
