#pragma once

#include <future>
#include <memory>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "../internal/bumbler.h"

namespace bumbler {

class thread_pool {
public:
    thread_pool(std::size_t pool_size);

    ~thread_pool();

    void stop(stop_mode stop_mode);

    template<typename F, typename... Rest>
    auto push(F&& f, Rest&&... rest) -> std::future<decltype(f(rest...))> {
        using taskType = decltype(f(rest...))();

        auto pck = std::make_shared<std::packaged_task<taskType>>([&f, &rest...]() {
            return f(std::forward<Rest>(rest)...);
        });

        io_service_.post([pck]() {
            // Suppress all exceptions.
            try {
                (*pck)();
            } catch (...) { }
        });

        return pck->get_future();
    }

    template<typename F>
    auto push(F&& f) -> std::future<decltype(f())> {
        using taskType = decltype(f())();

        auto pck = std::make_shared<std::packaged_task<taskType>>(std::forward<F>(f));

        io_service_.post([pck]() {
            // Suppress all exceptions.
            try {
                (*pck)();
            } catch (...) { }
        });

        return pck->get_future();
    }

private:
    boost::asio::io_service io_service_;
    boost::thread_group threads_;
    std::unique_ptr<boost::asio::io_service::work> work_;
};

}
