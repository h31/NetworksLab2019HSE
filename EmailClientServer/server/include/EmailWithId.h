#ifndef SERVER_EMAILWITHINFO_H
#define SERVER_EMAILWITHINFO_H

#include <model/model.h>

class EmailWithId {
private:
    uint32_t id;
    model::Email email;
    bool checked;

public:
    EmailWithId() = default;

    EmailWithId(uint32_t id, const model::Email &email);

    const uint32_t getId() const;

    const model::Email &getEmail() const;

    model::EmailInfo getInfo() const;

    bool operator == (const EmailWithId &other) const;
    bool operator < (const EmailWithId &other) const;

    void checkEmail();

    bool isChecked() const;
};

#endif //SERVER_EMAILWITHINFO_H
