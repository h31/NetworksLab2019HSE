//
// Created by Владислав Калинин on 11/02/2019.
//

#ifndef NETWORKS_UTILS_H
#define NETWORKS_UTILS_H

#include <string>
#include <climits>
#include <zconf.h>

std::string getWorkingPath() {
    char cwd[PATH_MAX];
    return std::string(getcwd(cwd, sizeof(cwd)));
}


#endif //NETWORKS_UTILS_H
