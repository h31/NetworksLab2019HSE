#ifndef SERVER_EMAILWITHINFO_H
#define SERVER_EMAILWITHINFO_H

#include <model/model.h>

class EmailWithInfo {
private:
    const uint32_t id = 0;
    const model::Email email;
    bool checked = false;

public:
    EmailWithInfo() = default;

    EmailWithInfo(uint32_t id, const model::Email &email);

    const uint32_t getId() const;

    const model::Email &getEmail() const;

    model::EmailInfo getInfo() const;

    bool operator == (const EmailWithInfo &other) const;
    bool operator < (const EmailWithInfo &other) const;

    void checkEmail();

    bool isChecked() const;
};

#endif //SERVER_EMAILWITHINFO_H
