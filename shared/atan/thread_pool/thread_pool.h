#pragma once

#include <future>
#include <functional>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <utility.h>

class thread_pool {
private:
    boost::asio::io_service io_service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    boost::thread_group threads_;
public:

    thread_pool(std::size_t pool_size);

    ~thread_pool();

    void stop(bool wait = false);

    template<typename F, typename... Rest>
    auto push(F &&f, Rest &&... rest) -> std::future<decltype(f(rest...))> {
        auto pck = std::make_shared<std::packaged_task<decltype(f(rest...))()>>(
                std::bind(std::forward<F>(f), std::forward<Rest>(rest)...)
        );

        io_service_.post([pck]() {
            // Suppress all exceptions.
            try {
                (*pck)();
            } catch(...) {}
        });

        return pck->get_future();
    }

    template<typename F>
    auto push(F && f) ->std::future<decltype(f())> {
        auto pck = std::make_shared<std::packaged_task<decltype(f())()>>(std::forward<F>(f));

        io_service_.post([pck]() {
            // Suppress all exceptions.
            try {
                (*pck)();
            } catch(...) {}
        });

        return pck->get_future();
    }
};