//
// Created by mikhail on 03.02.19.
//

#ifndef NETWORKSLAB2019HSE_CLIENT_H
#define NETWORKSLAB2019HSE_CLIENT_H


#include "Currency.h"

class Client {
public:
    Client(const std::string &hostname, uint16_t portno);

    const std::vector<Currency> list() const;

    bool addCurrency(const Currency &currency) const;

    bool addRate(const Currency &currency, int32_t new_rate) const;

    bool remove(const Currency &currency) const;

    Currency getCurrencyWithHistory(const Currency &currency) const;

    virtual ~Client();

private:
    const int sockfd;
    static const size_t BUFFER_INITIAL_LENGTH = 256;
    static const size_t CURRENCY_NAME_SIZE_IN_LIST = 16;
    static const int SECONDS_TO_WAIT_FOR_RESPONSE = 30;

    void write_end_of_message(std::vector<int8_t> &buffer) const;

    void write_command(std::vector<int8_t> &buffer, int32_t command_no) const;

    bool is_message_received(const std::vector<int8_t> &message) const;

    const std::vector<Currency> translate_list_message(std::vector<int8_t> &message) const;

    void remove_ending_symbols(std::vector<int8_t> &message) const;

    void write_string(std::vector<int8_t> &buffer, const std::string &currency_name) const;

    void write_int32(std::vector<int8_t> &buffer, int32_t rate) const;

    std::vector<int8_t> read_response() const;

    bool translate_add_message(std::vector<int8_t> &message) const;

    bool translate_remove_message(std::vector<int8_t> &message) const;

    std::vector<int32_t> translate_get_currency_history_message(std::vector<int8_t> &message) const;
};


#endif //NETWORKSLAB2019HSE_CLIENT_H
