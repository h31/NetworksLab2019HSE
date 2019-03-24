#include <utility>

#ifndef CLIENT_CLIENT_EXCEPTION_H
#define CLIENT_CLIENT_EXCEPTION_H

#include <exception>
#include <string>

class client_exception : std::runtime_error {
public:
    explicit client_exception(const std::string& message): runtime_error(message){};
};


#endif //CLIENT_CLIENT_EXCEPTION_H
