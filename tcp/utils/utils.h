#ifndef TCP_UTILS_H
#define TCP_UTILS_H

#include <string>
#include "../message/message.h"

bool WriteString(int sockfd, const std::string& str);

std::string ReadString(int sockfd);

bool WriteRequestMessage(int sockfd, const RequestMessage& message);

RequestMessage ReadRequestMessage(int sockfd);

#endif  // TCP_UTILS_H