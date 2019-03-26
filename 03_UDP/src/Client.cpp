#include "Client.h"

using std::string;
using std::vector;

Client::Client(string host_name, uint16_t port)
: _network(host_name, port)
{
}

calc_t Client::get_max_prime() const
{
    vector<uint64_t> message;
    message.push_back((uint64_t)CODE_GET_MAX_SIMPLE);

    vector<uint64_t> response;
    do {
        response = _network.send_message(message, sizeof(calc_t));
    } while (response.size() != 1);

    calc_t res = (calc_t)response[0];
    return res;
}

vector<calc_t> Client::get_last_n(calc_t n) const
{
    vector<uint64_t> message;
    message.push_back((uint64_t)CODE_GET_LAST_N);
    message.push_back((uint64_t)n);

    vector<uint64_t> response;
    do {
        response = _network.send_message(message, sizeof(calc_t));
    } while (response.size() == 0 || response[0] + 1 != response.size());

    vector<calc_t> res;
    for (size_t i = 1; i < response.size(); i++) {
        res.push_back((calc_t)response[i]);
    }
    return res;
}

void Client::calculate(calc_t n) const
{
    vector<uint64_t> message;
    message.push_back((uint64_t)CODE_CALCULATE);
    message.push_back((uint64_t)n);

    vector<uint64_t> response;
    do {
        response = _network.send_message(message, sizeof(calc_t));
    } while (response.size() != 1);

    calc_t start_num = (calc_t)response[0];

    vector<uint64_t> prime_nums = SimpleChecker::check_interval(start_num, n);

    message.clear();
    message.push_back((uint64_t)CODE_SEND_CALCULATED);
    message.push_back((uint64_t)n);
    message.push_back((uint64_t)start_num);
    message.push_back((uint64_t)prime_nums.size());
    for (uint64_t x : prime_nums) {
        message.push_back(x);
    }

    do {
        response = _network.send_message(message, sizeof(calc_t));
    } while (response.size() != 0);
}