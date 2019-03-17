#include "network/socket_exception.h"

namespace network {

    socket_exception::socket_exception(std::string msg) : msg_(std::move(msg)) {}

    const char *socket_exception::what() const noexcept {
        return msg_.data();
    }

} // namespace network 