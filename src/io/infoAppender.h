//
// Created by root on 24-4-23.
//

#ifndef VERTICES_INFOAPPENDER_H
#define VERTICES_INFOAPPENDER_H

#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "../common.h"

constexpr std::string startTag = "[VERTICES_OUTPUT_INFO_START]";
constexpr std::string endTag = "[VERTICES_OUTPUT_INFO_END]";



class InfoAppender {
private:
    static std::vector<std::string> tryRead(const std::string &filename);
public:
    static void append(const std::string &filename, const std::vector<std::string> &str);
};

#endif //VERTICES_INFOAPPENDER_H
