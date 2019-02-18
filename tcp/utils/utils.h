#ifndef TCP_UTILS_H
#define TCP_UTILS_H

#include <string>

bool WriteString(int sockfd, const std::string& str);

std::string ReadString(int sockfd);



#endif  // TCP_UTILS_H