//
// Created by root on 2024/4/4.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "filesystemLogger.h"

//std::filesystem::path *FilesystemLogger::p = nullptr;
//std::ofstream FilesystemLogger::out;

auto getFormattedTime() {
    const auto now = std::chrono::system_clock::now();
    const auto itt = std::chrono::system_clock::to_time_t(now);
    return std::put_time(std::localtime(&itt), "%Y%m%d%H%M%S");
}

std::string getAPathToNewTempFile() {
    std::string pathPrefix;
#ifdef _WIN32
    if (const char *temp = std::getenv("TEMP"); temp != nullptr) {
        pathPrefix = temp;
        pathPrefix += R"(\)";
    } else {
        pathPrefix = R"(C:\Windows\temp\)";
    }
#else
    pathPrefix = "/tmp/";
#endif
    std::ostringstream timeBuf;
    timeBuf << getFormattedTime();
    pathPrefix += ("vertices_tmp_" + timeBuf.str() + ".log");
    return pathPrefix;
}

void FilesystemLogger::writeInfoEntry(std::string_view entry, short level) noexcept {
    out << "[  INFO ] ";
    if (level >= 1 && level <= 10) {
#if VERTICES_CONFIG_CXX_STANDARD >= 20
        auto view = std::ranges::iota_view{1, level + 1};
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
        std::vector<int> view(level);
        std::iota(view.begin(), view.end(), 1);
#endif
        for (auto i: view) {
            out << "  ";
        }
    }
    out << entry << std::endl << std::flush;
};

void FilesystemLogger::writeWarnEntry(std::string_view entry, short level) noexcept {
    out << "[  WARN ] ";
    if (level >= 1 && level <= 10) {
#if VERTICES_CONFIG_CXX_STANDARD >= 20
        auto view = std::ranges::iota_view{1, level + 1};
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
        std::vector<int> view(level);
        std::iota(view.begin(), view.end(), 1);
#endif
        for (auto i: view) {
            out << "  ";
        }
    }
    out << entry << std::endl << std::flush;
};

void FilesystemLogger::writeErrorEntry(std::string_view entry, short level) noexcept {
    out << "[ ERROR ] ";
    if (level >= 1 && level <= 10) {
#if VERTICES_CONFIG_CXX_STANDARD >= 20
        auto view = std::ranges::iota_view{1, level + 1};
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
        std::vector<int> view(level);
        std::iota(view.begin(), view.end(), 1);
#endif
        for (auto i: view) {
            out << "  ";
        }
    }
    out << entry << std::endl << std::flush;
};

FilesystemLogger::FilesystemLogger() noexcept {
    if (p == nullptr) {
        p = new std::filesystem::path(getAPathToNewTempFile());
        std::cout << "FilesystemLogger: " << *p << std::endl;
        out = std::ofstream(*p);
        //out.open(p->string());
    }
}
