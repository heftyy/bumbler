#pragma once

#include <future>
#include <boost/any.hpp>

#include "../thread_pool/thread_pool.h"

namespace bumbler {

class actor_ref;

class dispatcher {
public:
    dispatcher(int size) {
        this->thread_pool_ = std::make_unique<thread_pool>(size);
    }

    ~dispatcher() {
        this->thread_pool_->stop(false);
    }

    void stop(bool wait = false) {
        this->thread_pool_->stop(wait);
    }

    std::future<int> push(const actor_ref& ref, const boost::any& data);

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
