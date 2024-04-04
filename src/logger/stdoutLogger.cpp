//
// Created by root on 2024/4/4.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#include "stdoutLogger.h"

void StdoutLogger::writeInfoEntry(std::string_view entry, short level) noexcept {
    char blanks[20 + 1] = {0};
    if (level >= 1 && level <= 10) {
        for (int i = 1; i <= level; i++) {
            blanks[(i - 1) * 2] = ' ';
            blanks[(i - 1) * 2 + 1] = ' ';
        }
        blanks[level * 2] = ' ';
    } else {
        blanks[0] = ' ';
    }
    fprintf(stdout, "[  INFO ]%s%s\n", blanks, entry.data());
};

void StdoutLogger::writeWarnEntry(std::string_view entry, short level) noexcept {
    char blanks[20 + 1] = {0};
    if (level >= 1 && level <= 10) {
        for (int i = 1; i <= level; i++) {
            blanks[(i - 1) * 2] = ' ';
            blanks[(i - 1) * 2 + 1] = ' ';
        }
        blanks[level * 2] = ' ';
    } else {
        blanks[0] = ' ';
    }
    fprintf(stdout, "[  WARN ]%s%s\n", blanks, entry.data());
};

void StdoutLogger::writeErrorEntry(std::string_view entry, short level) noexcept {
    char blanks[22] = {0};
    if (level >= 1 && level <= 10) {
        for (int i = 1; i <= level; i++) {
            blanks[(i - 1) * 2] = ' ';
            blanks[(i - 1) * 2 + 1] = ' ';
        }
        blanks[level * 2] = ' ';
    } else {
        blanks[0] = ' ';
    }
    fprintf(stderr, "[ ERROR ]%s%s\n", blanks, entry.data());
};

