#include "Network.h"

using std::vector;
using std::string;

Network::Network(string host_name, uint16_t port)
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {
        throw NetworkException("Could not open socket");
    }

    struct hostent *server = gethostbyname(host_name.c_str());

    if (server == NULL) {
        throw NetworkException("Could not find host");
    }

    struct sockaddr_in serv_addr;
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char*) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        throw NetworkException("Could not connect to server");
    }
}

void Network::send_int(uint64_t x, size_t size) const
{
    if (write(_sockfd, (char*)&x, size) < 0) {
        throw NetworkException("Could not send number");
    }
}

uint64_t Network::read_int(size_t size) const
{
    uint64_t x = 0;
    if (read(_sockfd, (char*)&x, size) < 0) {
        throw NetworkException("Could not read number");
    }
    return x;
}

void Network::send_vector(const vector<uint64_t> &v, size_t size_length, size_t size_num) const
{
    send_int(v.size(), size_length);
    for (uint64_t x : v) {
        send_int(x, size_num);
    }
}

vector<uint64_t> Network::read_vector(size_t size_length, size_t size_num) const
{
    vector<uint64_t> v;

    size_t n = read_int(size_length);
    for (size_t i = 0; i < n; i++) {
        v.push_back(read_int(size_num));
    }

    return v;
}