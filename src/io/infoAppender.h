//
// Created by root on 24-4-23.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_INFOAPPENDER_H
#define VERTICES_INFOAPPENDER_H

#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include "../common.h"
#include "../../lib/JSON/json.hpp"

const std::string startTag = "[VERTICES_OUTPUT_INFO_START]";
const std::string endTag = "[VERTICES_OUTPUT_INFO_END]";

constexpr int INT_INFO_START = 0;
constexpr int INT_INFO_COUNT = 13;
constexpr int UINT64_INFO_START = INT_INFO_START + INT_INFO_COUNT;
constexpr int UINT64_INFO_COUNT = 2;
constexpr int DOUBLE_INFO_START = UINT64_INFO_START + UINT64_INFO_COUNT;
constexpr int DOUBLE_INFO_COUNT = 23;

enum InfoType {
    INT_VERSION,
    INT_SUBVERSION,
    INT_THIS_BAND,
    INT_BAND_COUNT,
    INT_BAND_OVERLAPPING,
    INT_COUNT_X,
    INT_COUNT_Y,
    INT_FIELD_ITEM_COUNT,
    INT_GOODRAYS_COUNT,
    INT_FACES_IN_FIELD_COUNT,
    INT_MULTITHREAD_COUNT,
    INT_WAVELENGTH_LOW,
    INT_WAVELENGTH_HIGH,
    UINT64_LOCAL_TIMESTAMP,
    UINT64_UTC_TIMESTAMP,
    DOUBLE_FOV_X,
    DOUBLE_FOV_Y,
    DOUBLE_MIXER_RATIO_U,
    DOUBLE_MIXER_RATIO_D,
    DOUBLE_MIXER_RATIO_L,
    DOUBLE_MIXER_RATIO_R,
    DOUBLE_CAMERA_HEIGHT,
    DOUBLE_CAM_PLATE_HEIGHT,
    DOUBLE_CAM_PLATE_CENTER_X,
    DOUBLE_CAM_PLATE_CENTER_Y,
    DOUBLE_CAM_PLATE_CENTER_Z,
    DOUBLE_FIELD_BOX_MIN_X,
    DOUBLE_FIELD_BOX_MIN_Y,
    DOUBLE_FIELD_BOX_MIN_Z,
    DOUBLE_FIELD_BOX_MAX_X,
    DOUBLE_FIELD_BOX_MAX_Y,
    DOUBLE_FIELD_BOX_MAX_Z,
    DOUBLE_CAM_OX_X,
    DOUBLE_CAM_OX_Y,
    DOUBLE_CAM_OX_Z,
    DOUBLE_CAM_OY_X,
    DOUBLE_CAM_OY_Y,
    DOUBLE_CAM_OY_Z,
};

class InfoAppender {
private:
    std::string filename;
    std::vector<std::pair<int, bool> > intInfo;
    std::vector<std::pair<std::uint64_t, bool> > uint64Info;
    std::vector<std::pair<double, bool> > doubleInfo;

public:
    // u need to first tryRead() to see if it exists, then use getXXXInfo() to obtain inf
    void tryRead(std::string* ret);

    // u need to fisrt setXXXInfo(), then tryAppend() to write info into the file (only one chance tp append)
    void tryAppend() const noexcept;

    const std::string &getFilename() const noexcept { return filename; }

    InfoAppender() = delete;

    explicit InfoAppender(const std::string &filename) noexcept;

    [[nodiscard]] int getIntInfo(InfoType type) const noexcept;

    [[nodiscard]] uint64_t getUInt64Info(InfoType type) const noexcept;

    [[nodiscard]] double getDoubleInfo(InfoType type) const noexcept;

    InfoAppender &setIntInfo(InfoType type, int value) noexcept;

    InfoAppender &setUInt64Info(InfoType type, std::uint64_t value) noexcept;

    InfoAppender &setDoubleInfo(InfoType type, double value) noexcept;
};
#endif //VERTICES_INFOAPPENDER_H
