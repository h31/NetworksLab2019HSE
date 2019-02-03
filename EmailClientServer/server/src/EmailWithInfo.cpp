
#include <EmailWithInfo.h>

#include "EmailWithInfo.h"

EmailWithInfo::EmailWithInfo(const uint32_t id, const model::Email &email) : id(id), email(email), checked(false) {}

const uint32_t EmailWithInfo::getId() const {
    return id;
}

const model::Email &EmailWithInfo::getEmail() const {
    return email;
}

model::EmailInfo EmailWithInfo::getInfo() const {
    return model::EmailInfo(id, email.getAuthor(), email.getTheme());
}

bool EmailWithInfo::operator==(const EmailWithInfo &other) const {
    return id == other.id;
}

bool EmailWithInfo::operator<(const EmailWithInfo &other) const {
    return id < other.id;
}

void EmailWithInfo::checkEmail() {
    checked = true;
}

bool EmailWithInfo::isChecked() const {
    return checked;
}
