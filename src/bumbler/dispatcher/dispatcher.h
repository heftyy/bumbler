#pragma once

#include <future>
#include <boost/any.hpp>

#include "../thread_pool/thread_pool.h"

namespace bumbler {

class dispatcher {
public:
    dispatcher(int size) {
        this->thread_pool_ = std::make_unique<thread_pool>(size);
    }

    ~dispatcher() {
        this->thread_pool_->stop(stop_mode::IGNORE_QUEUE);
    }

    void stop(stop_mode stop_mode) {
        this->thread_pool_->stop(stop_mode);
    }

    template<typename F, typename ...Rest>
    auto push(F&& f, Rest&&... rest) -> std::future<decltype(f(rest...))> {
        return this->thread_pool_->push(std::forward<F>(f), std::forward<Rest>(rest)...);
    }

    template<typename F>
    auto push(F&& f) -> std::future<decltype(f())> {
        return this->thread_pool_->push(std::forward<F>(f));
    }

private:
    std::unique_ptr<thread_pool> thread_pool_;
};

}
