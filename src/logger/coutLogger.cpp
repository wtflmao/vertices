//
// Created by root on 2024/4/4.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "coutLogger.h"

void CoutLogger::writeInfoEntry(std::string_view entry, short level) noexcept {
    std::cout << "[  INFO ] ";
    if (level >= 1 && level <= 10) {
#if VERTICES_CONFIG_CXX_STANDARD >= 20
        auto view = std::ranges::iota_view{1, level + 1};
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
        std::vector<int> view(level);
        std::iota(view.begin(), view.end(), 1);
#endif
        for (auto i: view) {
            std::cout << "  ";
        }
    }
    std::cout << entry << std::endl << std::flush;
};

void CoutLogger::writeWarnEntry(std::string_view entry, short level) noexcept {
    std::cout << "[  WARN ] ";
    if (level >= 1 && level <= 10) {
#if VERTICES_CONFIG_CXX_STANDARD >= 20
        auto view = std::ranges::iota_view{1, level + 1};
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
        std::vector<int> view(level);
        std::iota(view.begin(), view.end(), 1);
#endif
        for (auto i: view) {
            std::cout << "  ";
        }
    }
    std::cout << entry << std::endl << std::flush;
};

void CoutLogger::writeErrorEntry(std::string_view entry, short level) noexcept {
    std::cerr << "[ ERROR ] ";
    if (level >= 1 && level <= 10) {
#if VERTICES_CONFIG_CXX_STANDARD >= 20
        auto view = std::ranges::iota_view{1, level + 1};
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
        std::vector<int> view(level);
        std::iota(view.begin(), view.end(), 1);
#endif
        for (auto i: view) {
            std::cerr << "  ";
        }
    }
    std::cerr << entry << "\a" << std::endl << std::flush;
};



