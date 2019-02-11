
#include <EmailWithId.h>

#include "EmailWithId.h"

EmailWithId::EmailWithId(const uint32_t id, const model::Email &email) : id(id), email(email), checked(false) {}

const uint32_t EmailWithId::getId() const {
    return id;
}

const model::Email &EmailWithId::getEmail() const {
    return email;
}

model::EmailInfo EmailWithId::getInfo() const {
    return model::EmailInfo(id, email.getAuthor(), email.getTheme());
}

bool EmailWithId::operator==(const EmailWithId &other) const {
    return id == other.id;
}

bool EmailWithId::operator<(const EmailWithId &other) const {
    return id < other.id;
}

void EmailWithId::checkEmail() {
    checked = true;
}

bool EmailWithId::isChecked() const {
    return checked;
}
