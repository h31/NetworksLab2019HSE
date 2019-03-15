//
// Created by mikhail on 03.02.19.
//

#ifndef NETWORKSLAB2019HSE_CLIENT_H
#define NETWORKSLAB2019HSE_CLIENT_H


#include <unordered_map>
#include <map>
#include "Currency.h"

class Client {
public:
    Client(const std::string &server_ip, uint16_t portno);

    const std::vector<Currency> list();

    bool addCurrency(const Currency &currency);

    bool addRate(const Currency &currency, int32_t new_rate);

    bool remove(const Currency &currency);

    Currency getCurrencyWithHistory(const Currency &currency);

    virtual ~Client();

private:
    struct sockaddr_in *si_other;
    const int sockfd;
    int32_t request_id = 0;
    int si_other_len;
    static const size_t BUFFER_INITIAL_LENGTH = 1024;
    static const size_t CURRENCY_NAME_SIZE_IN_LIST = 16;
    static const size_t MAX_SIZE_OF_PACKET = 508;
    static const size_t TIMOUT_SECONDS = 30;

    const std::vector<int8_t> get_message_with_request_id(const std::vector<int8_t> &message);

    const std::vector<int8_t> send_and_receive(const std::vector<int8_t> &message);

    void write_request_id(std::vector<int8_t> &buffer);

    bool is_all_packets_received(const std::map<int, std::vector<int8_t>> &packets, int number_of_packets) const;

    std::vector<int8_t>  construct_message_from_packets(const std::map<int, std::vector<int8_t>> &packets) const;

    void write_command(std::vector<int8_t> &buffer, int32_t command_no) const;

    const std::vector<Currency> translate_list_message(std::vector<int8_t> &message) const;

    void write_string(std::vector<int8_t> &buffer, const std::string &currency_name) const;

    void write_int32(std::vector<int8_t> &buffer, int32_t rate) const;

    std::vector<int8_t> read_response() const;

    bool translate_add_message(std::vector<int8_t> &message) const;

    bool translate_remove_message(std::vector<int8_t> &message) const;

    std::vector<int32_t> translate_get_currency_history_message(std::vector<int8_t> &message) const;

    void send_request(const std::vector<int8_t> &buffer) const;
};


#endif //NETWORKSLAB2019HSE_CLIENT_H
