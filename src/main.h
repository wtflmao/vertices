//
// Created by root on 2024/3/16.
//

/* Copyright 2024 wtflmao. All Rights Reserved.
 *
 * Distributed under MIT license.
 * See file LICENSE/LICENSE.MIT.md or copy at https://opensource.org/license/mit
 */

#ifndef VERTICES_MAIN_H
#define VERTICES_MAIN_H

#include "structures/ray.h"
#include "structures/triangle.h"
#include "structures/field.h"
#include "structures/camera.h"
#include "basic/coordTransform.h"
#include "basic/BRDF.h"

#include <iostream>
#include <array>
#include <vector>
#include <chrono>
#include <memory>


const Point NO_INTERSECT = Point(DBL_MAX, DBL_MAX, DBL_MAX);

#endif //VERTICES_MAIN_H