//
// Created by root on 2024/3/15.
//

#ifndef SRC_COMMON_H
#define SRC_COMMON_H

#include <memory>
template<class T> using ptr = std::unique_ptr<T>;
template <typename T, typename... Args> std::unique_ptr<T> make_ptr(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

#endif //SRC_COMMON_H