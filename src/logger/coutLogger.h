//
// Created by root on 2024/4/4.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_COUTLOGGER_H
#define VERTICES_COUTLOGGER_H

#include "loggingFacility.h"
#include <iostream>
#include <vector>

#if VERTICES_CONFIG_CXX_STANDARD >= 20
#include <ranges>
#elif VERTICES_CONFIG_CXX_STANDARD <= 17
#include <numeric>
#endif

class CoutLogger : public LoggingFacility {
public:
    void writeInfoEntry(std::string_view entry, short level) noexcept override;

    void writeWarnEntry(std::string_view entry, short level) noexcept override;

    void writeErrorEntry(std::string_view entry, short level) noexcept override;
};

#endif //VERTICES_COUTLOGGER_H
