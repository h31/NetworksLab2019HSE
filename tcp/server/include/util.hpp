#pragma once

#include <iostream>

void log(const std::string &s) {
    std::cout << s << std::endl;
}

void log_error(const std::string &s) {
    std::cerr << "Error: " << s << std::endl;
}

ssize_t socket_write_response(int socket_descriptor, struct dctp_response_header& response) {
    ssize_t c = write(socket_descriptor, &response, sizeof(response));

    log("Server: sent to socket "
        + std::to_string(socket_descriptor)
        + " response (size = " + std::to_string(c) + "): "
        + response_to_string(response));

    if (c < 0) {
        log_error("can't write to socket");
        return -1;
    }
    if (c == 0) {
        log_error("socket has closed.");
        return -1;
    }
    if (c > 0 && c != sizeof(response)) {
        log_error("data was not written fully.");
        return -1;
    }

    return c;
}

ssize_t socket_read_request(int socket_descriptor, struct dctp_request_header& request) {
    ssize_t read_bytes = 0;
    ssize_t struct_size = sizeof(request);
    while (read_bytes < struct_size) {
        ssize_t received = read(socket_descriptor, &request + read_bytes, static_cast<size_t>(struct_size - read_bytes));
        if (received < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                log_error("error while reading request");
                return -1;
            }
        } else if (received == 0) {
            log_error("socket was closed.");
            return -1;
        }
        read_bytes += received;
    }

    log("Server: socket " + std::to_string(socket_descriptor) + " sent request: " + request_to_string(request));

    return read_bytes;
}