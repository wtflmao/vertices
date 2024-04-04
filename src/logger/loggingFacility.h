//
// Created by root on 2024/4/4.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_LOGGINGFACILITY_H
#define VERTICES_LOGGINGFACILITY_H

#include <string_view>
#include <memory>

class LoggingFacility {
public:
    virtual ~LoggingFacility() = default;

    virtual void writeInfoEntry(std::string_view entry, short level = 0) = 0;

    virtual void writeWarnEntry(std::string_view entry, short level = 0) = 0;

    virtual void writeErrorEntry(std::string_view entry, short level = 0) = 0;
};

using Logger = std::shared_ptr<LoggingFacility>;

#endif //VERTICES_LOGGINGFACILITY_H
