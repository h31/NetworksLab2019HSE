#include "server.h"

bool operator < (segment one, segment other) {
    return one.left < other.left || (one.left == other.left && one.right < other.right);
}

Server::Server(int portno) {
    /* First call to socket() function */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct sockaddr_in serv_addr, cli_addr;

     /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);
    unprocessed.insert(segment(2, MAX_CALC));
}

Server::~Server() {
    pthread_mutex_destroy(&mutex);
}

calc_t Server::get_max() {
    pthread_mutex_lock(&mutex);
    calc_t ans = max_val;
    pthread_mutex_unlock(&mutex);
    return ans;
}

vector<calc_t> Server::get_last_n(size_t n) {
    vector<calc_t> ans;
    pthread_mutex_lock(&mutex);
    for (int i = max(calculated.size(), n) - n; i < calculated.size(); i++) {
        ans.push_back(calculated[i]);    
    }
    pthread_mutex_unlock(&mutex);
    return ans;
}

segment Server::ask_to_calculate(size_t n) {
    pthread_mutex_lock(&mutex);
    segment seg = *unprocessed.begin();
    unprocessed.erase(seg);
    segment ans = segment(seg.left, min(seg.right, seg.left + n));
    seg.left = ans.right;
    if (seg.left != seg.right) {
        unprocessed.insert(seg);
    }
    pthread_mutex_unlock(&mutex);
    return ans;
}

void Server::failed_to_calculate(segment seg) {
    pthread_mutex_lock(&mutex);
    auto it = unprocessed.upper_bound(seg);
    vector<segment> to_delete;
    if (it != unprocessed.end()) {
        if (it->left <= seg.right) {
            to_delete.push_back(*it);
            seg.right = it->right;
        }
    }
    if (it != unprocessed.begin()) {
        it--;
        if (it->right >= seg.left) {
            to_delete.push_back(*it);
            seg.left = it->left;
        }
    }
    for (segment d: to_delete) {
        unprocessed.erase(d);
    }
    unprocessed.insert(seg);
    pthread_mutex_unlock(&mutex);
}

void Server::add_calculated(vector<calc_t> _calculated) {
    pthread_mutex_lock(&mutex);
    for (calc_t d: _calculated) {
        calculated.push_back(d);
        max_val = max(max_val, d);
    }
    pthread_mutex_unlock(&mutex);
}

calc_t read_num(int sockfd) {
    calc_t buffer[2];
    bzero(buffer, sizeof(calc_t) * 2);
    ssize_t n;
    n = read(sockfd, buffer, sizeof(calc_t));
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    return buffer[0];
}

vector<calc_t> read_nums(int sockfd, calc_t num) {
    calc_t buffer[256];
    bzero(buffer, sizeof(calc_t) * 256);
    ssize_t n;
    vector<calc_t> ans;
    while (num > 0) {
        n = read(sockfd, buffer, sizeof(calc_t) * min((calc_t) 255, num));
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        for (int i = 0; i < min((calc_t) 255, num); i++) {
            ans.push_back(buffer[i]);
        }
        num = num - min(num, (calc_t) 255);
    }
    return ans;
}

void write_nums(int sockfd, vector<calc_t> nums) {
    calc_t buffer[256];
    for (int i = 0; i < nums.size();) {
        int j = 0;
        for (; j < 255 && i + j < nums.size(); j++) {
            buffer[j] = nums[i + j];
        }
        write(sockfd, buffer, sizeof(calc_t) * j);
    }
}

void *doit(void *args) {
    int sockfd = ((task_args*) args)->sockfd;
    Server* server = ((task_args*) args)->server;

    printf("wait, it's read\n");
    fflush(stdout);

    calc_t type = read_num(sockfd);
    if (type == 1) {
        write_nums(sockfd, {server->get_max()});
    } else if (type == 2) {
        calc_t n = read_num(sockfd);
        write_nums(sockfd, server->get_last_n(n));
    } else if (type == 3) {
        calc_t len = read_num(sockfd);
        segment seg = server->ask_to_calculate(len);
        write_nums(sockfd, {seg.left});
        calc_t n = read_num(sockfd);
        server->add_calculated(read_nums(sockfd, n));
    }
}

void Server::run() {
    while (true) {
        int sockfd;
        unsigned int clilen;
        struct sockaddr_in cli_addr;

        /* Now start listening for the clients, here process will
         * go in sleep mode and will wait for the incoming connection
        */
        listen(server_sockfd, 5);
        clilen = sizeof(cli_addr);


        printf("wait, it's accept\n");
        fflush(stdout);
        /* Accept actual connection from the client */
        sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (sockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t));
        pthread_create(thread, NULL, doit, new task_args(this, sockfd));
    }
}

task_args::task_args(Server* server, int sockfd): server(server), sockfd(sockfd) {}

segment::segment(calc_t left, calc_t right): left(left), right(right) {}