#pragma once

#include "../thread_pool/ctpl_stl.h"

class dispatcher {
public:
    dispatcher(int size) {
        this->thread_pool_.resize(size);
    }

    ~dispatcher() {
        this->thread_pool_.stop(false);
    }

    void stop(bool wait = false) {
        this->thread_pool_.stop(wait);
    }

    template<typename F, typename ...Rest>
    auto push(F && f, Rest&&... rest) ->std::future<decltype(f(0, rest...))> {
        return this->thread_pool_.push(f, rest...);
    }

    template<typename F>
    auto push(F && f) ->std::future<decltype(f(0))> {
        return this->thread_pool_.push(f);
    }

private:
    ctpl::thread_pool thread_pool_;
};

