#include "Network.h"

using std::vector;
using std::string;

Network::Network(string host_name, uint16_t port)
: _host_name(host_name), _port(port)
{}

int Network::try_send_message(char* message, size_t message_length, char* response) const
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        throw NetworkException("Could not open socket");
    }

    struct hostent *server = gethostbyname(_host_name.c_str());

    if (server == NULL) {
        throw NetworkException("Could not find host");
    }

    struct sockaddr_in serv_addr;
    memset((char*)&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    bcopy(server->h_addr, (char*) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(_port);

    struct timeval tv;
    tv.tv_sec = TIME_OUT_SEC;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        throw NetworkException("Could not set timeout");
    }

    int n;
    n = sendto(sockfd, (const char *)message, message_length, MSG_CONFIRM, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (n == -1) {
        close(sockfd);
        return -1;
    }

    socklen_t len;
    n = recvfrom(sockfd, (char *)response, MAX_LEN, MSG_WAITALL, (struct sockaddr *) &serv_addr, &len);
  
    close(sockfd);
    return n; 
}

uint64_t Network::get_check_sum(const vector<uint64_t> &v, size_t prefix_length) const 
{
    size_t len = v.size();
    if (len > prefix_length) {
        len = prefix_length;
    }

    uint64_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += v[i];
    }

    return sum;
}

size_t Network::data2message(const vector<uint64_t> &message_data, size_t size_num, char* message) const
{
    char* pointer = message;
    for (uint64_t x : message_data) {
        bcopy((const char *)&x, pointer, size_num);
        pointer += size_num;
    }

    uint64_t sum = get_check_sum(message_data, message_data.size());
    bcopy((const char *)&sum, pointer, size_num);
    pointer += size_num;

    return size_num * (message_data.size() + 1);
}

int Network::massage2data(char* response, int response_length, size_t size_num, vector<uint64_t> &response_data) const
{
    if (response_length == -1 || response_length == 0) {
        return -1;
    }

    if (response_length % size_num != 0) {
        return -1;
    }

    char* pointer = response;
    response_length /= size_num;
    while (response_length--) {
        uint64_t x;
        bcopy(pointer, (char *)&x, size_num);
        pointer += size_num;
        response_data.push_back(x);
    }

    if (response_data.back() != get_check_sum(response_data, response_data.size() - 1)) {
        return -1;
    }

    response_data.pop_back();

    return 0;
}

vector<uint64_t> Network::send_message(const vector<uint64_t> &message_data, size_t size_num) const
{
    char message[MAX_LEN];
    size_t message_length = data2message(message_data, size_num, message);

    char response[MAX_LEN];
    int response_length;
    vector<uint64_t> response_data;
    do {
        response_length = try_send_message(message, message_length, response);
    } while (massage2data(response, response_length, size_num, response_data) == -1);

    return response_data;
}