//
// Created by Владислав Калинин on 11/02/2019.
//

#ifndef NETWORKS_UTILS_H
#define NETWORKS_UTILS_H

#include <string>

void split(const std::string &txt, std::vector<std::string> &strs, char delimetr) {
    size_t pos = txt.find(delimetr);
    size_t initialPos = 0;
    strs.clear();

    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(delimetr, initialPos);
    }

    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));
}

std::string firstWord(const std::string &txt) {
    size_t pos = txt.find(' ');
    return txt.substr(0, pos);
}

#endif //NETWORKS_UTILS_H
