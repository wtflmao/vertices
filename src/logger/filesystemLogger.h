//
// Created by root on 2024/4/4.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_FILESYSTEMLOGGER_H
#define VERTICES_FILESYSTEMLOGGER_H

#include "loggingFacility.h"
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <ranges>
#include <cstdlib>

class FilesystemLogger : public LoggingFacility {
private:
    std::filesystem::path *p;
    std::ofstream out;

public:
    void writeInfoEntry(std::string_view entry, short level) noexcept override;

    void writeWarnEntry(std::string_view entry, short level) noexcept override;

    void writeErrorEntry(std::string_view entry, short level) noexcept override;

    /*~FilesystemLogger() noexcept override {
        if (p != nullptr) {
            if (out.is_open())
                out.close();
            delete p;
            p = nullptr;
        }
    }*/

    FilesystemLogger() noexcept;

    FilesystemLogger(const FilesystemLogger &other) = delete;

    FilesystemLogger(FilesystemLogger &&other) = delete;

    FilesystemLogger &operator=(const FilesystemLogger &other) = delete;

    FilesystemLogger &operator=(FilesystemLogger &&other) = delete;

};

#endif //VERTICES_FILESYSTEMLOGGER_H
