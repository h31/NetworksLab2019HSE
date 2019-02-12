#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

#include <pthread.h>

#include <set>
#include <vector>
#include <iostream>

using std::set;
using std::vector;
using std::max;
using std::min;
using std::cerr;
using std::endl;

typedef unsigned long long calc_t;
const calc_t MAX_CALC = 1e18;

struct segment {
    segment(calc_t left, calc_t right);
    calc_t left;
    calc_t right;
};

bool operator < (segment one, segment other);

class Server {
private:
    int server_sockfd;
    calc_t max_val = 0;
    vector<calc_t> calculated;
    set<segment> unprocessed;
    pthread_mutex_t mutex;
public:
    Server(int portno);
    ~Server();
    calc_t get_max();
    vector<calc_t> get_last_n(size_t n);
    segment ask_to_calculate(size_t n);
    void failed_to_calculate(segment seg);
    void add_calculated(vector<calc_t> calculated);
    void run();
};

struct task_args {
    Server* server;
    int sockfd;
    task_args(Server* server, int sockfd);
};