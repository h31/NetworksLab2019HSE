#include <utility>


#ifndef CLIENT_CONNECTING_EXCEPTION_H
#define CLIENT_CONNECTING_EXCEPTION_H

#include <exception>


class network_exception: public std::exception {
    private:
        std::string message;

    public:
        explicit network_exception(std::string message) : message(std::move(message)) {
        };

        const char* what() const noexcept override {
            return message.c_str();
        }
};

#endif //CLIENT_CONNECTING_EXCEPTION_H
