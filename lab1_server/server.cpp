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
    if (n <= 0) {
        throw rw_exception("ERROR reading from socket");
    }
    return buffer[0];
}

vector<calc_t> read_nums(int sockfd, calc_t num) {
    calc_t buffer[256];
    bzero(buffer, sizeof(calc_t) * 256);
    ssize_t n;
    vector<calc_t> ans;
    while (num > 0) {
        calc_t m = min((calc_t) 255, num);
        n = read(sockfd, buffer, sizeof(calc_t) * m);
        cerr << "bytes read: " << n << endl;
        if (n < 0) {
            throw rw_exception("ERROR reading from socket");
        }
        n /= 8;
        for (int i = 0; i < n; i++) {
            ans.push_back(buffer[i]);
        }
        num -= n;
    }
    return ans;
}

void write_nums(int sockfd, vector<calc_t> nums) {
    calc_t buffer[256];
    ssize_t n;
    for (int i = 0; i < nums.size();) {
        int j = 0;
        for (; j < 255 && i + j < nums.size(); j++) {
            buffer[j] = nums[i + j];
        }
        n = write(sockfd, buffer, sizeof(calc_t) * j);
        if (n < 0) {
            throw rw_exception("ERROR writing to socket");
        }
        i += j;
    }
}

void *doit(void *args) {
    int sockfd = ((task_args*) args)->sockfd;
    Server* server = ((task_args*) args)->server;

    while(true) {
 
        cout << "Waiting for read" << endl;

        calc_t type = 0;
        try {
            type = read_num(sockfd);
        } catch (const rw_exception &e) {
            cerr << "Connection closed" << endl;
            break;
        }
        cout << "Request type = " << type << endl;
        if (type == 1) {
            try {
                write_nums(sockfd, {server->get_max()});
            } catch(const rw_exception &e) {
                cerr << e.what() << endl;
                break;
            }
        } else if (type == 2) {
            calc_t n = read_num(sockfd);
            vector<calc_t> vt = server->get_last_n(n); 
            try {
                write_nums(sockfd, {vt.size()});
                write_nums(sockfd, vt);
            } catch(const rw_exception &e) {
                cerr << e.what() << endl;
                break;
            }
        } else if (type == 3) {
            calc_t len = 0;
            try {
                len = read_num(sockfd);
            } catch(const rw_exception &e) {
                cerr << e.what() << endl;
                break;
            }
            cout << "Asked to calculate segment with len = " << len << endl;
            segment seg = server->ask_to_calculate(len);
            cout << "Calculating segment from " << seg.left << " to " << seg.right << endl;
            try {
                write_nums(sockfd, {seg.left});
                calc_t n = read_num(sockfd);
                cout << "Got " << n << " numbers" << endl;
                vector<calc_t> vt = read_nums(sockfd, n);
                /*for (calc_t d: vt) {
                    cout << d << " ";
                }*/
                //cout << endl;
                server->add_calculated(vt);
            } catch(const rw_exception &e) {
                cerr << e.what() << endl;
                server->failed_to_calculate(seg);
                break;
            }
        } else {
            cerr << "Invalid command" << endl;
        }
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


        cout << "Wait for accept" << endl;
        /* Accept actual connection from the client */
        sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (sockfd < 0) {
            cerr << "ERROR on accept" << endl;
            continue;
        }

        pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t));
        pthread_create(thread, NULL, doit, new task_args(this, sockfd));
    }
}

task_args::task_args(Server* server, int sockfd): server(server), sockfd(sockfd) {}

segment::segment(calc_t left, calc_t right): left(left), right(right) {}

rw_exception::rw_exception(const string &what): runtime_error(what) {}