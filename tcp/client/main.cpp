#include "../message/include/message.h"
#include <cstdio>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <string.h>
#include <iostream>
#include <map>

using namespace std;

mutex m;

void receive_responses(int sockfd, map<int, Calculation*> &requests) {
    char buffer[256];

    while (true) {
        bzero(buffer, 256);
        read(sockfd, buffer, sizeof(Response));
        Result result = Result::Deserialize(buffer);

        m.lock();
        auto it = requests.find(result.GetId());
        if (it != requests.end()) {
            int arg_left = it->second->GetArgLeft();
            int arg_right = it->second->GetArgRight();
            char operation = it->second->GetOperation();

            cout << "< ";
            if (operation == 's') {
                cout << "sqrt " << arg_left << " = " << result.GetValue() << endl;
            } else if (operation == '!') {
                cout << arg_left << "! = " << result.GetValue() << endl;
            } else {
                cout << arg_left << " " << operation << " "  << arg_right << " = " << result.GetValue() << endl;
            }
        }
        m.unlock();
    }
}

int main(int argc, char* argv[]) {
    int sockfd;
    int n;
    uint16_t portno;
    struct sockaddr_in serv_addr{};
    struct hostent *server;

    map<int, Calculation*> requests;

    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = (uint16_t) atoi(argv[2]);


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

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    thread t = thread(receive_responses, sockfd, requests);

    string s;
    cout << "> ";
    while (getline(cin, s)) {
        int arg_left, arg_right;
        char operation;
        Calculation* calculation;
        if (sscanf(s.c_str(), "%d %c %d", &arg_left, &operation, &arg_right) != 3) {
            if (sscanf(s.c_str(), "%d!", &arg_left) == 1) {
                calculation = new Calculation('!', arg_left, 0);
            } else if (sscanf(s.c_str(), "sqrt %d", &arg_left) == 1) {
                calculation = new Calculation('s', arg_left, 0);
            } else {
                cout << "Invalid input!\n> ";
                continue;
            }
        } else {
            calculation = new Calculation(operation, arg_left, arg_right);
        }

        char* serialized = calculation->Serialize();
        n = write(sockfd, serialized, sizeof(Calculation));
        delete serialized;

        if (n < 0) {
            cerr << "ERROR writing to socket\n";
            exit(1);
        }

        bzero(buffer, 256);
        n = read(sockfd, buffer, sizeof(Response));

        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        Response response = Response::Deserialize(buffer);

        m.lock();
        requests.insert(pair<int, Calculation*>(response.GetId(), calculation));
        m.unlock();

        cout << calculation->GetArgLeft() << calculation->GetOperation() << calculation->GetArgRight() << endl;
        delete calculation;
        cout << "> ";
    }
    return 0;
}