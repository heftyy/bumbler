#pragma once

#include "../utility.h"
#include "../thread_pool/thread_pool.h"

class dispatcher {
public:
    dispatcher(int size) {
        this->thread_pool_ = utility::make_unique<thread_pool>(size);
    }

    ~dispatcher() {
        this->thread_pool_->stop(false);
    }

    void stop(bool wait = false) {
        this->thread_pool_->stop(wait);
    }

    template<typename F, typename ...Rest>
    auto push(F && f, Rest&&... rest) ->std::future<decltype(f(rest...))> {
        return this->thread_pool_->push(std::forward<F>(f), std::forward<Rest>(rest)...);
    }

    template<typename F>
    auto push(F && f) ->std::future<decltype(f())> {
        return this->thread_pool_->push(std::forward<F>(f));
    }

private:
    std::unique_ptr<thread_pool> thread_pool_;
};
