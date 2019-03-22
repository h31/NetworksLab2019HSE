#include "../include/server.h"
#include "../../message/include/message.h"

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Server::Server(uint16_t port)
{
    port_ = port;

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd_ < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *)&serv_addr_, sizeof(serv_addr_));

    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_addr.s_addr = INADDR_ANY;
    serv_addr_.sin_port = htons(port_);

    if (bind(sockfd_, (struct sockaddr *)&serv_addr_, sizeof(serv_addr_)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }
}

void Server::Run()
{

    listen(sockfd_, 10); // could be changed
    socklen_t clilen = sizeof(cli_addr_);
    while (!exit_)
    {
        int newsockfd = accept(sockfd_, (struct sockaddr *)&cli_addr_, &clilen);

        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            if (exit_)
                break;
            continue;
        }

        client_threads_.emplace_back(&Server::ClientLifeCycle, this, newsockfd);
    }
}

void Server::Stop()
{
    shutdown(sockfd_, SHUT_RDWR);
    exit_ = 1;
}

void Server::ClientLifeCycle(int newsockfd)
{
    char *buf = new char[sizeof(Calculation)];

    while (!exit_)
    {
        try
        {
            int n = read(newsockfd, buf, sizeof(Calculation));

            if (n <= 0)
            {
                break;
            }

            Calculation calculation = Calculation::Deserialize(buf);

            char operation = calculation.GetOperation();
            if (operation == 's' || operation == '!')
            {
                operations_threads_.emplace_back(&Server::ProcessLongOperation, this, operation, calculation.GetArgLeft(), newsockfd);
            }
            else if (operation == '+' || operation == '-' || operation == '*' || operation == '/')
            {
                double result = Server::ProcessOperation(operation, calculation.GetArgLeft(), calculation.GetArgRight());
                calculation.SetResult(result);
                SendCalculationResult(newsockfd, calculation);
            }
        }
        catch (std::exception e)
        {
            break;
        }
    }

    shutdown(newsockfd, SHUT_RD);
    close(newsockfd);

    delete buf;
}

Server::~Server()
{
    for (auto &thread : client_threads_)
    {
        thread.join();
    }
    for (auto &thread : operations_threads_)
    {
        thread.join();
    }
    close(sockfd_);
}

double Server::ProcessOperation(char operation, int left_arg, int right_arg)
{
    if (operation == '+')
    {
        return left_arg + right_arg;
    }
    else if (operation == '-')
    {
        return left_arg - right_arg;
    }
    else if (operation == '*')
    {
        return left_arg * right_arg;
    }
    else if (operation == '/')
    {
        return (1.0 * left_arg) / right_arg;
    }

    return 0;
}

void Server::ProcessLongOperation(char operation, int arg, int newsockfd)
{
    double result = 0;

    std::thread new_thread;
    if (operation == '!')
    {
        result = Server::GetFactorial(arg);
    }
    else if (operation == 's')
    {
        result = Server::GetSqrt(arg);
    }

    Calculation calculation(operation, arg, 0, result);
    SendCalculationResult(newsockfd, calculation);
}

double Server::GetSqrt(int n)
{
    return std::sqrt(n);
}

double Server::GetFactorial(int n)
{
    if (n < 0)
    {
        return 0;
    }
    else if (n <= 1)
    {
        return 1;
    }
    else
    {
        return n * GetFactorial(n - 1);
    }
}

bool Server::SendCalculationResult(int newsockfd, const Calculation &calculation) const
{
    try
    {
        char *serialized = calculation.Serialize();
        int n = write(newsockfd, serialized, sizeof(Calculation));

        if (n <= 0)
        {
            return false;
        }

        delete serialized;

        return true;
    }
    catch (...)
    {
        return false;
    }
}