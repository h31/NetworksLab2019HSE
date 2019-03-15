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
#include <unordered_map>
#include <algorithm>
#include <map>

const std::vector<int8_t> Client::get_message_with_request_id(const std::vector<int8_t> &message) {
    std::vector<int8_t> send_buffer;
    write_request_id(send_buffer);
    send_buffer.insert(send_buffer.end(), message.begin(), message.end());
    return send_buffer;
}

const std::vector<int8_t> Client::send_and_receive(const std::vector<int8_t> &message) {
    bool got_all_message = false;
    std::vector<int8_t> response;
    while(!got_all_message) {
        send_request(get_message_with_request_id(message));

        try {
            response = read_response();
        } catch (const char *timeout_exception) {
            continue;
        }
        got_all_message = true;
    }
    return response;
}

const std::vector<Currency> Client::list() {
    int32_t command_no = 0;
    auto write_buffer = std::vector<int8_t>();

    write_command(write_buffer, command_no);

    auto response = send_and_receive(write_buffer);
    return translate_list_message(response);
}


bool Client::addCurrency(const Currency &currency) {
    int32_t command_no = 1;
    auto write_buffer = std::vector<int8_t>();

    write_command(write_buffer, command_no);
    write_string(write_buffer, currency.get_name());
    write_int32(write_buffer, currency.get_current_rate());

    auto response = send_and_receive(write_buffer);
    return translate_add_message(response);
}

bool Client::remove(const Currency &currency) {
    int32_t command_no = 2;
    auto write_buffer = std::vector<int8_t>();

    write_command(write_buffer, command_no);
    write_string(write_buffer, currency.get_name());

    auto response = send_and_receive(write_buffer);
    return translate_remove_message(response);
}

bool Client::addRate(const Currency &currency, int32_t new_rate) {
    int32_t command_no = 3;
    auto write_buffer = std::vector<int8_t>();

    write_command(write_buffer, command_no);
    write_string(write_buffer, currency.get_name());
    write_int32(write_buffer, new_rate);

    auto response = send_and_receive(write_buffer);
    return translate_add_message(response);
}

Currency Client::getCurrencyWithHistory(const Currency &currency) {
    auto write_buffer = std::vector<int8_t>();
    int32_t command_no = 4;

    write_command(write_buffer, command_no);
    write_string(write_buffer, currency.get_name());

    auto response = send_and_receive(write_buffer);
    return Currency(currency.get_name(), translate_get_currency_history_message(response));
}

Client::Client(const std::string &server_ip, uint16_t portno) : sockfd(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct timeval tv{};
    tv.tv_sec = TIMOUT_SECONDS;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error: cannot set timeout for socket");
        exit(1);
    }

    si_other = new sockaddr_in();
    bzero((char *) si_other, sizeof(si_other));
    si_other->sin_family = AF_INET;
    si_other->sin_port = htons(portno);
    if (inet_aton(server_ip.c_str(), &(si_other->sin_addr)) == 0) {
        perror("inet_aton() failed\n");
        exit(1);
    }
    si_other_len = sizeof(*si_other);
}

Client::~Client() {
    close(sockfd);
    delete(si_other);
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

void Client::write_request_id(std::vector<int8_t> &buffer) {
    request_id++;
    insert_bytes(buffer, request_id);
}

void Client::write_command(std::vector<int8_t> &buffer, int32_t command_no) const {
    insert_bytes(buffer, command_no);
}

bool Client::translate_remove_message(std::vector<int8_t> &message) const {
    auto it = message.begin();
    return read_bool(it);
}

std::vector<int32_t> Client::translate_get_currency_history_message(std::vector<int8_t> &message) const {
    std::vector<int32_t> history;
    for (auto it = message.begin(); it < message.end();) {
        history.push_back(read_int32(it));
    }
    return history;
}

bool Client::translate_add_message(std::vector<int8_t> &message) const {
    auto it = message.begin();
    return read_bool(it);
}

const std::vector<Currency> Client::translate_list_message(std::vector<int8_t> &message) const {
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

void Client::write_string(std::vector<int8_t> &buffer, const std::string &currency_name) const {
    std::string string_to_write(currency_name);
    string_to_write.resize(CURRENCY_NAME_SIZE_IN_LIST, 0);
    buffer.insert(buffer.end(), string_to_write.begin(), string_to_write.end());
}

void Client::write_int32(std::vector<int8_t> &buffer, int32_t rate) const {
    insert_bytes(buffer, rate);
}

std::vector<int8_t> Client::read_response() const {
    bool all_packets_received = false;
    auto packets = std::map<int, std::vector<int8_t>>();

    while (!all_packets_received) {
        auto read_buffer = std::vector<int8_t>(BUFFER_INITIAL_LENGTH);
        ssize_t bytes_number = recvfrom(sockfd, read_buffer.data(), read_buffer.size(), 0,
                (struct sockaddr *) si_other, (socklen_t *) &si_other_len);

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            perror("Timout reached for recvfrom, probably packages was lost or request wasn't received, sending again."
                   " Error:");
            throw "TIMEOUT";
        }
        if (bytes_number == 0) {
            perror("Connection was closed by server, please try later");
            exit(1);
        }
        if (bytes_number < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        read_buffer.resize(static_cast<unsigned long>(bytes_number));
        auto it = read_buffer.begin();
        int32_t read_request_id = read_int32(it);
        if (read_request_id != request_id) {
            continue;
        }
        int32_t read_number_of_packets = read_int32(it);
        int32_t packet_id = read_int32(it);
        auto packet_message = std::vector<int8_t>(it, read_buffer.end());
        packets[packet_id] = packet_message;
        all_packets_received = is_all_packets_received(packets, read_number_of_packets);
    }
    return construct_message_from_packets(packets);
}

std::vector<int8_t> Client::construct_message_from_packets(const std::map<int, std::vector<int8_t>> &packets) const {
    std::vector<int8_t> message;
    for (const auto& packet : packets) {
        message.insert(message.end(), packet.second.begin(), packet.second.end());
    }
    return message;
}

bool Client::is_all_packets_received
(const std::map<int, std::vector<int8_t>> &packets, int number_of_packets) const {
    for (int packet_id = 0; packet_id < number_of_packets; packet_id++) {
        if (packets.count(packet_id) == 0) {
            return false;
        }
    }
    return true;
}

void Client::send_request(const std::vector<int8_t> &buffer) const {
    if (buffer.size() > MAX_SIZE_OF_PACKET) {
        perror("ERROR sending data, packet size is more then maximum size");
        exit(1);
    }
    auto return_code = sendto(sockfd, buffer.data(), buffer.size(), 0,
                              (struct sockaddr *) si_other, static_cast<socklen_t>(si_other_len));
    if (return_code < 0) {
        perror("ERROR sending data to socket");
        exit(1);
    }
}
