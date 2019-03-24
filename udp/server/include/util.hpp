#pragma once

#include <iostream>

void log(const std::string &s) {
    std::cout << s << std::endl;
}

void log_error(const std::string &s) {
    std::cerr << "Error: " << s << std::endl;
}

ssize_t socket_write_response(int socket_descriptor, struct dctp_response_header& response, sockaddr_in& client_addr) {
    socklen_t client_size = sizeof(client_addr);
    ssize_t sent_bytes = sendto(
            socket_descriptor,
            &response,
            sizeof(response),
            0,
            (struct sockaddr *) &client_addr,
            client_size
            );

    log("Server: sent to socket "
        + std::to_string(socket_descriptor)
        + " response (size = " + std::to_string(sent_bytes) + "): "
        + response_to_string(response));

    if (sent_bytes < 0) {
        log_error("can't write to socket");
        return -1;
    }
    if (sent_bytes == 0) {
        log_error("socket has closed.");
        return -1;
    }
    if (sent_bytes > 0 && sent_bytes != sizeof(response)) {
        log_error("data was not written fully.");
        return -1;
    }

    return sent_bytes;
}

ssize_t socket_read_request(int socket_descriptor, struct dctp_request_header& request, sockaddr_in& client_addr) {
    socklen_t client_size = sizeof(client_addr);
    ssize_t read_bytes = recvfrom(
            socket_descriptor,
            (void *) &request,
            sizeof(request),
            0,
            (struct sockaddr *) &client_addr,
            &client_size
            );

    log("Server: socket " + std::to_string(socket_descriptor) + " sent request: " + request_to_string(request));

    if (read_bytes < 0) {
        log_error("error while reading request");
        return -1;
    }
    if (read_bytes == 0) {
        log_error("socket has closed.");
        return -1;
    }
    if (read_bytes > 0 && read_bytes != sizeof(request)) {
        log_error("data was not read fully (read " + std::to_string(read_bytes) + " bytes).");
        return -1;
    }

    return read_bytes;
}