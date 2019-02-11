#include <iostream>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <vector>
#include <unordered_set>
#include <functional>

int write_operation_code(int sockfd, char code) {
    if (code < 0 || code > 6) {
        return -1;
    }
    return static_cast<int>(write(sockfd, &code, 1));
}

int write_uint32_t(int sockfd, uint32_t value) {
    value = htole32(value);
    int err = static_cast<int>(write(sockfd, &value, 4));
    if (err < 0) {
        return err;
    }
    return 0;
}

int write_string(int sockfd, const std::string &to_write) {
    int err = write_uint32_t(sockfd, static_cast<uint32_t>(to_write.length()));
    if (err < 0) {
        return err;
    }
    return static_cast<int>(write(sockfd, to_write.c_str(), strlen(to_write.c_str())));
}

int read_uint32_t(int sockfd, uint32_t* size) {
    *size = 0;
    int err = static_cast<int>(read(sockfd, size, 4));
    if (err < 0) {
        return err;
    }
    *size = le32toh(*size);
    return 0;
}

int read_string(int sockfd, std::string* to_read) {
    uint32_t size = 0;
    int err = read_uint32_t(sockfd, &size);
    if (err < 0) {
        return err;
    }
    char* str = (char *)malloc(size + 1);
    err = static_cast<int>(read(sockfd, str, size));
    if (err < 0) {
        return err;
    }
    str[size] = '\0';
    *to_read = std::string(str);
    free(str);
    return 0;
}

int log_in_response(int sockfd) {
    char result = 0;
    int err = static_cast<int>(read(sockfd, &result, 1));
    if (err < 0) {
        return err;
    }

    if (result != 0) {
        std::cout << "Error: " << (int)result << std::endl;
        return -result;
    }

    std::cout << "Succeed" << std::endl;

    uint32_t last_right = 0;
    err = read_uint32_t(sockfd, &last_right);
    if (err < 0) {
        return err;
    }

    uint32_t last_total = 0;
    err = read_uint32_t(sockfd, &last_total);
    if (err < 0) {
        return err;
    }

    std::cout << "Welcome back! Last time you have answered right " << last_right << " questions out of " << last_total << std::endl;
    return 0;
}

int get_questions(int sockfd, std::unordered_set<uint32_t>* result) {
    uint32_t amount = 0;
    int err = read_uint32_t(sockfd, &amount);
    if (err < 0) {
        return err;
    }

    std::cout << "There are " << amount << " of tests:" << std::endl;
    for (int i = 0; i < amount; i++) {
        uint32_t number = 0;
        err = read_uint32_t(sockfd, &number);
        if (err < 0) {
            return err;
        }
        result->insert(number);
        std::cout << "The test number " << number << std::endl;
        std::string description;
        err = read_string(sockfd, &description);
        if (err < 0) {
            return err;
        }
        std::cout << "Description: " << description << std::endl;
    }
    return 0;
}

int start_test(int sockfd, uint32_t number) {
    int err = write_operation_code(sockfd, 2);
    if (err < 0) {
        return err;
    }
    err = write_uint32_t(sockfd, number);
    if (err < 0) {
        return err;
    }
    char result = 0;
    err = static_cast<int>(read(sockfd, &result, 1));
    if (err < 0) {
        return err;
    }
    if (result == 0) {
        std::cout << "Succeed" << std::endl;
    } else {
        std::cout << "Got error: " << (int)result << std::endl;
    }
    return 0;
}


std::function<void(int)> shutdown_handler;
void signal_handler(int signal) {
    shutdown_handler(signal);
}

int main(int argc, char *argv[]) {
    int sockfd, err;
    uint16_t portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = (uint16_t) atoi(argv[2]);

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    shutdown_handler = [sockfd](int s) {
        write_operation_code(sockfd, 6);
        std::cout << "Shutting down" << std::endl;
        exit(0);
    };

    signal(SIGINT, signal_handler);

    /* Log in or sign up?
    */

    std::cout << "Are you a new user? Type in \"yes\" if you are a new user. " << std::endl;
    std::string s;
    std::cin >> s;

    if (strcmp(s.c_str(), "yes") == 0) {
        err = write_operation_code(sockfd, 0);
    } else {
        err = write_operation_code(sockfd, 1);
    }

    if (err < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    std::cout << "Write your login: " << std::endl;
    std::cin >> s;

    err = write_string(sockfd, s);

    if (err < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    err = log_in_response(sockfd);

    if (err < 0) {
        perror("ERROR reading the socket response.");
        exit(1);
    }

    std::unordered_set<uint32_t > set{};

    err = get_questions(sockfd, &set);

    if (err < 0) {
        perror("ERROR reading the socket response.");
        exit(1);
    }

    while (true) {
        std::cout << "Which test do you want to solve? Just write the number: " << std::endl;
        uint32_t number;
        std::cin >> number;

        err = start_test(sockfd, number);

        if (err < 0) {
            perror("ERROR sending the test number.");
            exit(1);
        }

        char code = 0;
        err = static_cast<int>(read(sockfd, &code, 1));
        if (err < 0) {
            perror("ERROR receiving message code");
            exit(1);
        }

        while (code == 3) {
            uint32_t quest_number;
            err = read_uint32_t(sockfd, &quest_number);
            if (err < 0) {
                perror("ERROR getting info from server.");
                exit(1);
            }
            std::string question;
            read_string(sockfd, &question);
            std::cout << "Question " << quest_number << ". " << question << std::endl;
            std::string answer;
            std::cin >> answer;
            err = write_operation_code(sockfd, 4);
            if (err < 0) {
                perror("ERROR writing to server");
                exit(1);
            }
            err = write_string(sockfd, answer);
            if (err < 0) {
                perror("ERROR writing to server");
                exit(1);
            }
            err = static_cast<int>(read(sockfd, &code, 1));
            if (err < 0) {
                perror("ERROR receiving message code");
                exit(1);
            }
        }

        if (code == 5) {
            uint32_t right;
            err = read_uint32_t(sockfd, &right);
            if (err < 0) {
                perror("ERROR getting info from server.");
                exit(1);
            }
            uint32_t total;
            err = read_uint32_t(sockfd, &total);
            if (err < 0) {
                perror("ERROR getting info from server.");
                exit(1);
            }
            std::cout << "Hey! You have finished your test with the result " << right << " right out of " << total
                      << " total." << std::endl;
        }
    }
}