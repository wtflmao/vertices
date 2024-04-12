//
// Created by root on 24-4-12.
//

#ifndef VERTICES_FUNCTIONWRAPPER_H
#define VERTICES_FUNCTIONWRAPPER_H

#include <memory>
#include <utility>

class FunctionWrapper final {
private:
    struct impl_base {
        virtual void call() = 0;

        virtual ~impl_base() = default;
    };

    std::unique_ptr<impl_base> impl;

    template<typename F>
    struct impl_type final : impl_base {
        F f;

        explicit impl_type(F &&f_) : f(std::move(f_)) {
        }

        void call() override { f(); }
    };

public:
    template<typename F>
    explicit FunctionWrapper(F &&f) : impl(new impl_type<F>(std::move(f))) {
    }

    void operator()() const { impl->call(); }

    FunctionWrapper() = default;

    FunctionWrapper(FunctionWrapper &&other) noexcept : impl(std::move(other.impl)) {
    }

    FunctionWrapper &operator=(FunctionWrapper &&other) noexcept {
        impl = std::move(other.impl);
        return *this;
    }

    FunctionWrapper(FunctionWrapper &) = delete;

    FunctionWrapper(const FunctionWrapper &) = delete;

    FunctionWrapper &operator=(const FunctionWrapper &) = delete;
};


#endif //VERTICES_FUNCTIONWRAPPER_H
