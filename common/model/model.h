#ifndef COMMON_MODEL_MODEL_H
#define COMMON_MODEL_MODEL_H

#include <string>

namespace model {
    class Email {
    private:
        const std::string theme;
        const std::string body;
        const std::string author;
        const std::string recipient;

    public:
        Email() = default;

        Email(const std::string &theme, const std::string &body, const std::string &author, const std::string &recipient);

        const std::string &getTheme() const;

        const std::string &getBody() const;

        const std::string &getAuthor() const;

        const std::string &getRecipient() const;
    };

    class EmailInfo {
    private:
        uint32_t id;
        std::string author;
        std::string theme;
    public:
        EmailInfo(uint32_t id, const std::string &author, const std::string &theme);

        uint32_t getId() const;

        const std::string &getAuthor() const;

        const std::string &getTheme() const;
    };
}


#endif //COMMON_MODEL_MODEL_H
