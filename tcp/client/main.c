#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT                "8881"
#define server_address      "localhost"
#define cl_error(s)         printf("%s", s)

int get_message(char *buffer) {
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
}

void exit_cl(int sock_fd) {
    char    str[256];
    int     rsnd = 0;
    ssize_t n    = 0;
    bzero(str, 256);
    strcpy(str, "GBYE ");
    if ((n = write(sock_fd, str, strlen(str))) < 0) {
        cl_error("Can't safely logout: error writing to server");
    } else if (n == 0) {
        cl_error("Broken connection");
        close(sock_fd);
        exit(0);
    }
}

void send_request(int sock_fd, char *request, char *buffer) {
    ssize_t n = 0;
    if ((n = write(sock_fd, request, strlen(request))) < 0) {
        cl_error("Can't register: error writing to server");
        close(sock_fd);
        exit(1);
    } else if (n == 0) {
        cl_error("Broken connection");
        close(sock_fd);
        exit(0);
    }

    bzero(buffer, 256);
    if ((n = read(sock_fd, buffer, 255)) < 0) {
        cl_error("Can't register: error getting respond from server");
        close(sock_fd);
        exit(1);
    } else if (n == 0) {
        cl_error("Broken connection");
        close(sock_fd);
        exit(0);
    }
}

int main() {
    int                errno;
    int                sock_fd;
    ssize_t            n;
    char               str[256];
    char               buffer[256];
    struct hostent     *server;
    int                number_of_tests;
    struct sockaddr_in serv_addr;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cl_error("Error creating a socket");
        exit(1);
    }

    if ((server = gethostbyname(server_address)) == NULL) {
        cl_error("No such host");
        close(sock_fd);
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons((uint16_t) atoi(PORT));

    /* Now connect to the server */
    if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cl_error("Error connecting");
        close(sock_fd);
        exit(1);
    }

    do {
        printf("Do you have login? <yes/no>: ");
        get_message(buffer);
    } while ((strncmp(buffer, "yes", 3) || buffer[3] != '\n')
             && (strncmp(buffer, "no", 2) || buffer[2] != '\n'));

    if (!strncmp(buffer, "no", 2)) {
        do {
            printf("Please, register with login (max 20 characters): ");
            get_message(buffer);
            strcpy(str, "REGI ");
            strcat(str, buffer);

            if ((n = write(sock_fd, str, strlen(str))) < 0) {
                cl_error("Can't register: error writing to server");
                continue;
            } else if (n == 0) {
                cl_error("Broken connection");
                close(sock_fd);
                exit(0);
            }
            bzero(buffer, 256);
            if ((n = read(sock_fd, buffer, 255)) < 0) {
                cl_error("Can't register: error getting respond from server");
                continue;
            } else if (n == 0) {
                cl_error("Broken connection");
                close(sock_fd);
                exit(0);
            }

            if (!strcmp("REGI UE \n", buffer)) {
                cl_error("Can't register: login already in use, try another\n");
                continue;
            }

            if (strcmp("REGI OK \n", buffer) != 0) {
                cl_error("Can't register: unknown error code\n");
                continue;
            }
            break;
        } while (1);
    } else {
        do {
            printf("Please, write your login to continue: ");
            get_message(buffer);
            strcpy(str, "AUTH ");
            strcat(str, buffer);

            if ((n = write(sock_fd, str, strlen(str))) < 0) {
                cl_error("Can't login: error writing to server\n");
                continue;
            } else if (n == 0) {
                cl_error("Broken connection");
                close(sock_fd);
                exit(0);
            }

            bzero(buffer, 256);
            if ((n = read(sock_fd, buffer, 255)) < 0) {
                cl_error("Can't login: error getting respond from server\n");
                continue;
            } else if (n == 0) {
                cl_error("Broken connection");
                close(sock_fd);
                exit(0);
            }

            if (!strcmp("AUTH UN \n", buffer)) {
                printf(buffer);
                cl_error("\n");
                cl_error("Can't login: no such login registered\n");
                continue;
            }

            if (strcmp("AUTH OK \n", buffer)) {
                cl_error("Can't login: unknown error code\n");
                continue;
            }
            break;
        } while (1);
    }
    cl_error("Success!\n");

    do {
        cl_error("Available commands: \"exit\", \"list\", \"test\" and \"last\"\n");
        get_message(buffer);
        if (!strcmp("exit\n", buffer)) {
            exit_cl(sock_fd);
            break;
        } else if (!strcmp("list\n", buffer)) {
            number_of_tests = 0;
            bzero((char *) &buffer, sizeof(buffer));
            send_request(sock_fd, "LIST \n", buffer);

            if (!strcmp("LIST NA \n", buffer)) {
                cl_error("Can't get list of tests: User Not Authorized\n");
                continue;
            }

            if (strncmp("LIST OK ", buffer, 8)) {
                cl_error("Can't get list of tests: unknown error code\n");
                continue;
            }

            printf("You can choose from these tests:\n");
            for (int i = 8; i < strlen(buffer) - 1; ++i) {
                char id[10]   = "\0";
                char name[50] = "\0";
                while (buffer[i] != ' ') {
                    char curChar[2] = "\0";
                    curChar[0] = buffer[i];
                    strcat(id, curChar);
                    i++;
                }
                i++;
                while (buffer[i] != '|') {
                    char curChar[2] = "\0";
                    curChar[0] = buffer[i];
                    strcat(name, curChar);
                    i++;
                }
                printf("%s: %s\n", id, name);
                number_of_tests++;
            }
            printf("\n");
        } else if (!strcmp("test\n", buffer)) {
            if (!number_of_tests) {
                printf("Run \"list\" first\n");
                continue;
            }
            int id;
            do {
                printf("Please, write number of the test you want to use (%d - %d): ", 1, number_of_tests);
                get_message(buffer);
                id = atoi(buffer);
            } while (id < 1 || id > number_of_tests);

            char request[256] = "\0";
            char id_str[10]   = "\0";
            strcat(request, "TEST ");
            sprintf(id_str, "%d\n", id);
            strcat(request, id_str);
            send_request(sock_fd, request, buffer);

            if (!strcmp("TEST NA \n", buffer)) {
                cl_error("Can't get test: User Not Authorized\n");
                continue;
            }

            if (strncmp("TEST OK ", buffer, 8)) {
                cl_error("Can't get test: unknown error code\n");
                continue;
            }

            do {
                int  j         = 8;
                char text[100] = "\0";
                while (buffer[j] != '&') {
                    char curChar[2] = "\0";
                    curChar[0] = buffer[j];
                    strcat(text, curChar);
                    j++;
                }
                j++;
                printf("%s\n", text);
                number_of_tests = 0;
                for (; j < strlen(buffer) - 1; ++j) {
                    char id[10]   = "\0";
                    char name[50] = "\0";
                    while (buffer[j] != ' ') {
                        char curChar[2] = "\0";
                        curChar[0] = buffer[j];
                        strcat(id, curChar);
                        j++;
                    }
                    j++;
                    while (buffer[j] != '|') {
                        char curChar[2] = "\0";
                        curChar[0] = buffer[j];
                        strcat(name, curChar);
                        j++;
                    }
                    printf("%s: %s\n", id, name);
                    number_of_tests++;
                }
                do {
                    printf("Please, take an answer (%d - %d): ", 0, number_of_tests - 1);
                    get_message(buffer);
                    id = atoi(buffer);
                } while (id < 0 || id > number_of_tests - 1);

                char request[256] = "\0";
                char id_str[10]   = "\0";
                strcat(request, "ANSW ");
                sprintf(id_str, "%d\n", id);
                strcat(request, id_str);
                send_request(sock_fd, request, buffer);

                if (!strcmp("ANSW NA \n", buffer)) {
                    cl_error("Can't get results: User Not Authorized\n");
                    close(sock_fd);
                    exit(1);
                }

                if (!strncmp("ANSW TE ", buffer, 8)) {
                    printf("Results:\n");
                    int  j         = 8;
                    char text[100] = "\0";
                    number_of_tests = 0;
                    for (; j < strlen(buffer) - 1; ++j) {
                        char id[10]   = "\0";
                        char name[50] = "\0";
                        while (buffer[j] != ' ') {
                            char curChar[2] = "\0";
                            curChar[0] = buffer[j];
                            strcat(id, curChar);
                            j++;
                        }
                        j++;
                        while (buffer[j] != '|') {
                            char curChar[2] = "\0";
                            curChar[0] = buffer[j];
                            strcat(name, curChar);
                            j++;
                        }
                        printf("%s: %s\n", id, name);
                        number_of_tests++;
                    }
                    break;
                }

                if (strncmp("ANSW OK ", buffer, 8)) {
                    cl_error("Can't get test: unknown error code\n");
                    close(sock_fd);
                    exit(1);
                }
            } while (1);
        } else if (!strcmp("last\n", buffer)) {
            send_request(sock_fd, "LAST \n", buffer);

            if (!strcmp("LAST NA \n", buffer)) {
                cl_error("Can't get last test results: User Not Authorized\n");
                continue;
            }

            if (!strcmp("LAST NT \n", buffer)) {
                cl_error("Can't get last test results: test history is empty\n");
                continue;
            }

            if (strncmp("LAST OK ", buffer, 8)) {
                cl_error("Can't get last test results: unknown error\n");
                continue;
            }
            printf("Results:\n");
            int j = 8;
            for (; j < strlen(buffer) - 1; ++j) {
                char id[10]   = "\0";
                char name[50] = "\0";
                while (buffer[j] != ' ') {
                    char curChar[2] = "\0";
                    curChar[0] = buffer[j];
                    strcat(id, curChar);
                    j++;
                }
                j++;
                while (buffer[j] != '|') {
                    char curChar[2] = "\0";
                    curChar[0] = buffer[j];
                    strcat(name, curChar);
                    j++;
                }
                printf("%s: %s\n", id, name);
            }
            printf("\n");
        } else if (!strcmp("\n", buffer)) {
            continue;
        } else {
            cl_error("Unknown command\n");
        }
    } while (1);

    close(sock_fd);
    return 0;
}