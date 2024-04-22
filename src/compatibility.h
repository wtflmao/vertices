//
// Created by root on 24-4-22.
//

#ifndef VERTICES_COMPATIBILITY_H
#define VERTICES_COMPATIBILITY_H

#ifndef VERTICES_CONFIG_CXX_STANDARD
// remember to edit CMakeLists.txt if you want to change the c++ standard
#define VERTICES_CONFIG_CXX_STANDARD 17
#endif

#ifdef VERTICES_CONFIG_CXX_STANDARD
#if VERTICES_CONFIG_CXX_STANDARD > 32
#pragma message "[  Warn ] C++ standard is too new for me. C++20 or 23 or 26 would be better."
#include <format>
#include <numbers>
#include <ranges>
#elif VERTICES_CONFIG_CXX_STANDARD == 26 || (VERTICES_CONFIG_CXX_STANDARD == 23 || VERTICES_CONFIG_CXX_STANDARD == 20)
#include <format>
#include <numbers>
#include <ranges>
#elif VERTICES_CONFIG_CXX_STANDARD == 17
#include <cmath>
#else
#error "[ Abort ] Unsupported C++ standard(<=C++14), you need a compiler that at least supports C++17. Reminder: C++20 or 23 or 26 would be better."
#endif
#else
#error "[ Abort ] No VERTICES_CONFIG_CXX_STANDARD defined in src/common.h"
#endif

#endif //VERTICES_COMPATIBILITY_H
