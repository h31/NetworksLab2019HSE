//
// Created by mikhail on 03.02.19.
//

#include <netinet/in.h>
#include <memory.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <Client.h>
#include <cstring>
#include <algorithm>

const std::vector<Currency> Client::list() const {
    auto write_buffer = std::vector<int8_t>();
    int32_t command_no = 0;

    write_command(write_buffer, command_no);
    write_end_of_message(write_buffer);
    write_request(write_buffer);

    std::vector<int8_t> response = read_response();
    return translate_list_message(response);
}


bool Client::addCurrency(const Currency &currency) const {
    auto write_buffer = std::vector<int8_t>();
    int32_t command_no = 1;

    write_command(write_buffer, command_no);
    write_string(write_buffer, currency.get_name());
    write_int32(write_buffer, currency.get_current_rate());
    write_end_of_message(write_buffer);
    write_request(write_buffer);

    std::vector<int8_t> response = read_response();
    return translate_add_message(response);
}

bool Client::remove(const Currency &currency) const {
    auto write_buffer = std::vector<int8_t>();
    int32_t command_no = 2;

    write_command(write_buffer, command_no);
    write_string(write_buffer, currency.get_name());
    write_end_of_message(write_buffer);
    write_request(write_buffer);

    std::vector<int8_t> response = read_response();
    return translate_remove_message(response);
}

bool Client::addRate(const Currency &currency, int32_t new_rate) const {
    auto write_buffer = std::vector<int8_t>();
    int32_t command_no = 3;

    write_command(write_buffer, command_no);
    write_string(write_buffer, currency.get_name());
    write_int32(write_buffer, new_rate);
    write_end_of_message(write_buffer);
    write_request(write_buffer);

    std::vector<int8_t> response = read_response();
    return translate_add_message(response);
}

Currency Client::getCurrencyWithHistory(const Currency &currency) const {
    auto write_buffer = std::vector<int8_t>();
    int32_t command_no = 4;

    write_command(write_buffer, command_no);
    write_string(write_buffer, currency.get_name());
    write_end_of_message(write_buffer);
    write_request(write_buffer);

    std::vector<int8_t> response = read_response();
    return Currency(currency.get_name(), translate_get_currency_history_message(response));
}

Client::Client(const std::string &hostname, uint16_t portno) : sockfd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct hostent *server = gethostbyname(hostname.c_str());

    if (server == nullptr) {
        unsigned int addr = inet_addr(hostname.c_str());
        server = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
        if (server == nullptr) {
            perror("ERROR, no such host");
            exit(1);
        }
    }

    struct sockaddr_in server_addr{};
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &server_addr.sin_addr.s_addr, (size_t) server->h_length);
    server_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
}

Client::~Client() {
    close(sockfd);
}

template<class T>
void insert_bytes(std::vector<int8_t> &container, const T &value) {
    const auto *bytes = reinterpret_cast<const int8_t *>(&value);
    container.insert(container.end(), bytes, bytes + sizeof(T));
}

int32_t read_int32(std::vector<int8_t>::iterator &it) {
    int8_t bytes[] = {it[0], it[1], it[2], it[3]};
    int32_t val = *((int*)bytes);
    it += sizeof(int32_t);
    return val;
}

bool read_bool(std::vector<int8_t>::iterator &it) {
    bool val = it[0];
    it += sizeof(bool);
    return val;
}

std::string read_string(std::vector<int8_t>::iterator &it, size_t len) {
    std::string str(it, it + len);
    str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
    it += len;
    return str;
}

void Client::write_end_of_message(std::vector<int8_t> &buffer) const {
    insert_bytes(buffer, (int8_t) '\\');
    insert_bytes(buffer, (int8_t) 0);
}

void Client::write_command(std::vector<int8_t> &buffer, int32_t command_no) const {
    insert_bytes(buffer, command_no);
}

bool Client::is_message_received(const std::vector<int8_t> &message) const {
    return message.size() >= 2 && message[message.size() - 2] == '\\' && message[message.size() - 1] == 0;
}

bool Client::translate_remove_message(std::vector<int8_t> &message) const {
    remove_ending_symbols(message);
    auto it = message.begin();
    return read_bool(it);
}

std::vector<int32_t> Client::translate_get_currency_history_message(std::vector<int8_t> &message) const {
    remove_ending_symbols(message);
    std::vector<int32_t> history;
    for (auto it = message.begin(); it < message.end();) {
        history.push_back(read_int32(it));
    }
    return history;
}

bool Client::translate_add_message(std::vector<int8_t> &message) const {
    remove_ending_symbols(message);
    auto it = message.begin();
    return read_bool(it);
}

const std::vector<Currency> Client::translate_list_message(std::vector<int8_t> &message) const {
    remove_ending_symbols(message);
    std::vector<Currency> currencies;
    for (auto it = message.begin(); it < message.end();) {
        std::string currency_name = read_string(it, CURRENCY_NAME_SIZE_IN_LIST);
        int32_t current_rate = read_int32(it);
        bool has_change = read_bool(it);
        int32_t absolute_change = read_int32(it);
        int32_t relative_change = read_int32(it);
        if (has_change) {
            currencies.emplace_back(currency_name, current_rate, absolute_change, relative_change);
        } else {
            currencies.emplace_back(currency_name, current_rate);
        }
    }
    return currencies;
}

void Client::remove_ending_symbols(std::vector<int8_t> &message) const {
    message.erase(message.end() - 2, message.end());
}

void Client::write_string(std::vector<int8_t> &buffer, const std::string &currency_name) const {
    std::string string_to_write(currency_name);
    string_to_write.resize(CURRENCY_NAME_SIZE_IN_LIST, 0);
    buffer.insert(buffer.end(), string_to_write.begin(), string_to_write.end());
}

void Client::write_int32(std::vector<int8_t> &buffer, int32_t rate) const {
    insert_bytes(buffer, rate);
}

std::vector<int8_t> Client::read_response() const {
    bool message_received = false;
    auto message = std::vector<int8_t>();

    while (!message_received) {
        auto read_buffer = std::vector<int8_t>(BUFFER_INITIAL_LENGTH);
        ssize_t bytes_number = read(sockfd, read_buffer.data(), read_buffer.size());

        if (bytes_number == 0) {
            perror("Connection was closed by server, please try later");
            exit(1);
        }
        if (bytes_number < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        read_buffer.resize(static_cast<unsigned long>(bytes_number));
        message.insert(message.end(), read_buffer.begin(), read_buffer.end());
        message_received = is_message_received(message);
    }
    return message;
}

void Client::write_request(const std::vector<int8_t> &buffer) const {
    auto written_piece = write(sockfd, buffer.data(), buffer.size());
    if (written_piece < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}
