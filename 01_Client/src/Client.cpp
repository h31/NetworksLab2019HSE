#include "Client.h"

using std::string;
using std::vector;

Client::Client(string host_name, uint16_t port)
: _network(host_name, port)
{
}

calc_t Client::get_max_prime() const
{
    _network.send_int(CODE_GET_MAX_SIMPLE, sizeof(CODE_GET_MAX_SIMPLE));
    return (calc_t)_network.read_int(sizeof(calc_t));
}

vector<calc_t> Client::get_last_n(count_t n) const
{
    _network.send_int(CODE_GET_LAST_N, sizeof(CODE_GET_LAST_N));
    _network.send_int(n, sizeof(n));

    vector<calc_t> res;
    vector<uint64_t> response = _network.read_vector(sizeof(count_t), sizeof(calc_t));
    for (uint64_t x : response)
        res.push_back((calc_t)x);
    return res;
}

void Client::calculate(count_t n) const
{
    _network.send_int(CODE_CALCULATE, sizeof(CODE_CALCULATE));
    _network.send_int(n, sizeof(n));

    calc_t start_num = (calc_t)_network.read_int(sizeof(calc_t));
    vector<uint64_t> prime_nums = SimpleChecker::check_interval(start_num, n);

    _network.send_vector(prime_nums, sizeof(count_t), sizeof(calc_t));
}