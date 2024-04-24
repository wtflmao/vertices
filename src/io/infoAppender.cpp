//
// Created by root on 24-4-23.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "infoAppender.h"

// function that reads from end of the file looking for startTag and endTag
std::ifstream::pos_type findTagInFile(std::ifstream &file, const std::string &startTag_, const std::string &endTag_) {
    const std::streamoff fileSize = file.seekg(0, std::ios::end).tellg();

    // calculate the size of the largest block to search based on the tags length
    const std::streamoff searchBlockSize = static_cast<std::streamoff>(std::max(startTag_.size(), endTag_.size()) * 2);

    std::string buffer(searchBlockSize, '\0');

    for (std::streamoff pos = fileSize - searchBlockSize; pos >= 0; pos -= searchBlockSize / 2) {
        file.seekg(pos);
        file.read(&buffer[0], searchBlockSize);

        // search for tags in the buffer
        auto startTagPos = buffer.find(startTag_);
        auto endTagPos = buffer.rfind(endTag_);

        // check if found both tags and they're in correct order
        if (startTagPos != std::string::npos && endTagPos != std::string::npos && startTagPos < endTagPos)
            return pos + startTagPos; // return start position of the tag in file
    }

    // return failure
    return -1;
}

void InfoAppender::tryAppend() const noexcept {
    // check if already appended
    std::ifstream file_(filename, std::ios::binary);
    if (!file_.is_open()) {
        coutLogger->writeErrorEntry("Failed to open file by read|binary at InfoAppender::tryAppend(): " + filename);
        exit(93);
    }
    if (auto tagPos = findTagInFile(file_, startTag, endTag); tagPos != -1) {
        file_.close();
        coutLogger->writeErrorEntry("Already appended at InfoAppender::tryAppend(): " + filename);
        return;
    }

    std::ofstream file(filename, std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        coutLogger->writeErrorEntry("Failed to open file by append|binary at InfoAppender::tryAppend(): " + filename);
        exit(91);
    }

    nlohmann::json JSONObj;
    nlohmann::json intJSONObj;
    nlohmann::json uint64JSONObj;
    nlohmann::json doubleJSONObj;

    for (int i = 0; i < INT_INFO_COUNT; i++) {
        intJSONObj[i] = {std::to_string(intInfo[i].first), intInfo[i].second};
    }
    for (int i = 0; i < UINT64_INFO_COUNT; i++) {
        uint64JSONObj[i] = {std::to_string(uint64Info[i].first), uint64Info[i].second};
    }
    for (int i = 0; i < DOUBLE_INFO_COUNT; i++) {
        std::stringstream ss;
        ss << std::setprecision(20) << doubleInfo[i].first;
        doubleJSONObj[i] = {ss.str(), doubleInfo[i].second};
    }
    JSONObj["int"] = intJSONObj;
    JSONObj["uint64"] = uint64JSONObj;
    JSONObj["double"] = doubleJSONObj;

    std::stringstream data;
    data << std::endl << JSONObj.dump(-1) << std::endl << std::endl;
    file.write(startTag.c_str(), sizeof(char) * std::strlen(startTag.c_str()));
    // JSON data should be only in one single line
    file.write(data.str().c_str(), sizeof(char) * std::strlen(data.str().c_str()));
    file.write(endTag.c_str(), sizeof(char) * std::strlen(endTag.c_str()));
    file.close();
}

void InfoAppender::tryRead() noexcept {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        coutLogger->writeErrorEntry("Failed to open file by read|binary at InfoAppender::tryRead(): " + filename);
        exit(92);
    }

    std::vector<std::string> data;
    if (auto tagPos = findTagInFile(file, startTag, endTag); tagPos != -1) {
        // get it! read the data
        file.seekg(tagPos);
        std::string line;

        while (std::getline(file, line) && line != endTag) {
            data.push_back(line);
        }

        // now data SHOULD have 3 lines (startTag, JSONdata, endTag)
        if (data.size() == 2) {
            // how come?!! no appended data but have two tags available
            coutLogger->writeWarnEntry("Tag found, but no appended data is available.");
        } else if (data.size() == 3) {
            if (data.at(0) == startTag && data.at(2) == endTag) {
                // read JSON data
                nlohmann::json JSONObj = nlohmann::json::parse(data.at(1));
                nlohmann::json intJSONObj = JSONObj["int"];
                nlohmann::json uint64JSONObj = JSONObj["uint64"];
                nlohmann::json doubleJSONObj = JSONObj["double"];

                for (int i = 0; i < INT_INFO_COUNT; i++) {
                    intInfo[i].first = intJSONObj[i][0].get<int>();
                    intInfo[i].second = intJSONObj[i][1].get<bool>();
                }
                for (int i = 0; i < UINT64_INFO_COUNT; i++) {
                    uint64Info[i].first = uint64JSONObj[i][0].get<std::uint64_t>();
                    uint64Info[i].second = uint64JSONObj[i][1].get<bool>();
                }
                for (int i = 0; i < DOUBLE_INFO_COUNT; i++) {
                    doubleInfo[i].first = doubleJSONObj[i][0].get<double>();
                    doubleInfo[i].second = doubleJSONObj[i][1].get<bool>();
                }
            } else {
                coutLogger->writeErrorEntry("Unexpected tag and data at InfoAppender::tryRead(): " + filename);
                exit(94);
            }
        } else {
            coutLogger->writeErrorEntry("Unexpected lines of data at InfoAppender::tryRead(): " + filename);
            exit(95);
        }
    }

    file.close();
}

int InfoAppender::getIntInfo(const InfoType type) const noexcept {
    if (type < INT_INFO_START || type >= INT_INFO_START + INT_INFO_COUNT) {
        coutLogger->writeErrorEntry("Invalid InfoType at InfoAppender::getIntInfo(): " + std::to_string(type));
        return -1;
    }
    return intInfo.at(type - INT_INFO_START).first;
}

std::uint64_t InfoAppender::getUInt64Info(const InfoType type) const noexcept {
    if (type < UINT64_INFO_START || type >= UINT64_INFO_START + UINT64_INFO_COUNT) {
        coutLogger->writeErrorEntry("Invalid InfoType at InfoAppender::getUInt64Info(): " + std::to_string(type));
        return -1;
    }
    return uint64Info.at(type - UINT64_INFO_START).first;
}

double InfoAppender::getDoubleInfo(const InfoType type) const noexcept {
    if (type < DOUBLE_INFO_START || type >= DOUBLE_INFO_START + DOUBLE_INFO_COUNT) {
        coutLogger->writeErrorEntry("Invalid InfoType at InfoAppender::getDoubleInfo(): " + std::to_string(type));
        return -1;
    }
    return doubleInfo.at(type - DOUBLE_INFO_START).first;
}


InfoAppender &InfoAppender::setIntInfo(const InfoType type, const int value) noexcept {
    if (type < INT_INFO_START || type >= INT_INFO_START + INT_INFO_COUNT) {
        coutLogger->writeErrorEntry("Invalid InfoType at InfoAppender::setIntInfo(): " + std::to_string(type));
        return *this;
    }
    intInfo.at(type - INT_INFO_START).first = value;
    intInfo.at(type - INT_INFO_START).second = true;
    return *this;
}

InfoAppender &InfoAppender::setUInt64Info(const InfoType type, const std::uint64_t value) noexcept {
    if (type < UINT64_INFO_START || type >= UINT64_INFO_START + UINT64_INFO_COUNT) {
        coutLogger->writeErrorEntry("Invalid InfoType at InfoAppender::setUInt64Info(): " + std::to_string(type));
        return *this;
    }
    uint64Info.at(type - UINT64_INFO_START).first = value;
    uint64Info.at(type - UINT64_INFO_START).second = true;
    return *this;
}

InfoAppender &InfoAppender::setDoubleInfo(const InfoType type, const double value) noexcept {
    if (type < DOUBLE_INFO_START || type >= DOUBLE_INFO_START + DOUBLE_INFO_COUNT) {
        coutLogger->writeErrorEntry("Invalid InfoType at InfoAppender::setDoubleInfo(): " + std::to_string(type));
        return *this;
    }
    doubleInfo.at(type - DOUBLE_INFO_START).first = value;
    doubleInfo.at(type - DOUBLE_INFO_START).second = true;
    return *this;
}

InfoAppender::InfoAppender(const std::string &filename_) noexcept {
    filename = filename_;
    for (int i = 0; i < INT_INFO_COUNT; i++)
        intInfo.emplace_back(0, false);
    for (int i = 0; i < UINT64_INFO_COUNT; i++)
        uint64Info.emplace_back(0ull, false);
    for (int i = 0; i < DOUBLE_INFO_COUNT; i++)
        doubleInfo.emplace_back(0.0f, false);
}
