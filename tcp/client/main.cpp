#include "../message/include/message.h"
#include <cstdio>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <iostream>

using namespace std;

int sockfd;

void close_connection (int s) {

}

int main(int argc, char* argv[]) {
    int n;
    uint16_t portno;
    struct sockaddr_in serv_addr{};
    struct hostent *server;

    char buffer[256];

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

    /* Now ask for a message from the user, this message
       * will be read by server
    */

    string s;
    printf("> ");
    while (cin >> s) {
        int arg_left, arg_right;
        char operation;
        Calculation* calculation;
        if (sscanf(s.c_str(), "%d %c %d", &arg_left, &operation, &arg_right) != 3) {
            if (sscanf(s.c_str(), "%d!", &arg_left) == 1) {
                calculation = new Calculation('!', arg_left, 0);
            } else if (sscanf(s.c_str(), "sqrt %d", &arg_left) == 1) {
                calculation = new Calculation('s', arg_left, 0);
            } else {
                printf("Invalid input!\n");
                continue;
            }
        } else {
            calculation = new Calculation(operation, arg_left, arg_right);
        }

        /*
        n = write(sockfd, buffer, strlen(buffer));

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);

        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        */
        printf("(%d %c %d\n", calculation->GetArgLeft(), calculation->GetOperation(), calculation->GetArgRight());
        delete calculation;
        printf("> ");
    }
    return 0;
}