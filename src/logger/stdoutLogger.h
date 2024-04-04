//
// Created by root on 2024/4/4.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_STDOUTLOGGER_H
#define VERTICES_STDOUTLOGGER_H

#include "loggingFacility.h"
#include <cstdio>

class StdoutLogger : public LoggingFacility {
public:
    void writeInfoEntry(std::string_view entry, short level) noexcept override;

    void writeWarnEntry(std::string_view entry, short level) noexcept override;

    void writeErrorEntry(std::string_view entry, short level) noexcept override;
};

#endif //VERTICES_STDOUTLOGGER_H
