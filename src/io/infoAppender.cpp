//
// Created by root on 24-4-23.
//

#include "infoAppender.h"

void InfoAppender::append(const std::string &filename, const std::vector<std::string> &str) noexcept {
    std::ofstream file(filename, std::ios::app | std::ios::binary);
    if (file.is_open()) {
        file.write(startTag.c_str(), sizeof(char) * std::strlen(startTag.c_str()));

        for (auto &data: str) {
            file.write(data.c_str(), sizeof(char) * std::strlen(data.c_str()));
        }
        file.write(endTag.c_str(), sizeof(char) * std::strlen(endTag.c_str()));
    } else {
        coutLogger->writeErrorEntry("Failed to open file at InfoAppender::append(): " + filename);
        exit(91);
    }
    file.close();
}

// function that reads from end of the file looking for startTag and endTag
std::ifstream::pos_type findTagInFile(std::ifstream& file, const std::string& startTag_, const std::string& endTag_)
{
    const std::streamoff fileSize = file.seekg(0, std::ios::end).tellg();

    // calculate the size of the largest block to search based on the tags length
    const std::streamoff searchBlockSize = static_cast<std::streamoff>(std::max(startTag_.size(), endTag_.size()) * 2);

    std::string buffer(searchBlockSize, '\0');

    for (std::streamoff pos = fileSize - searchBlockSize; pos >= 0; pos -= searchBlockSize / 2)
    {
        file.seekg(pos);
        file.read(&buffer[0], searchBlockSize);

        // search for tags in the buffer
        auto startTagPos = buffer.find(startTag_);
        auto endTagPos = buffer.rfind(endTag_);

        // check if found both tags and they're in correct order
        if (startTagPos != std::string::npos && endTagPos != std::string::npos && startTagPos < endTagPos)
            return pos + startTagPos;  // return start position of the tag in file
    }

    // return failure
    return -1;
}

[[nodiscard]] std::vector<std::string> InfoAppender::tryRead(const std::string &filename) noexcept {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        coutLogger->writeErrorEntry("Failed to open file at InfoAppender::tryRead(): " + filename);
        exit(92);
    }

    std::vector<std::string> data;
    auto tagPos = findTagInFile(file, startTag, endTag);
    if (tagPos != -1) {
        // get it! read the data
        file.seekg(tagPos);
        std::string line;

        while (std::getline(file, line) && line != endTag) {
            data.push_back(line);
        }
    }
    file.close();
    return data;
}
