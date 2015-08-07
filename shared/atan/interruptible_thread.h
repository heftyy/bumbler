#pragma once

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <stdexcept>
#include <string>
#include <atomic>
#include <boost/log/trivial.hpp>

class interrupt_thread_error : public std::runtime_error {
public:
    interrupt_thread_error(const std::string& message) : std::runtime_error(message) { };
};


class cancellation_point {
public:
    cancellation_point() : stop_(false) { }

    void cancel() {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
        cond_.notify_all();
    }

    template<typename P>
    void wait(const P& period) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (stop_ || cond_.wait_for(lock, period) == std::cv_status::no_timeout) {
            stop_ = false;
            throw interrupt_thread_error("thread stopped");
        }
    }

private:
    bool stop_;
    std::mutex mutex_;
    std::condition_variable cond_;
};


class interruptible_thread {
public:
    interruptible_thread() { }

    ~interruptible_thread() {
        stop();
    }

    template<typename Function>
    void start(Function&& fun, long initial_delay = 0, long period = 0) {
        thread_ = std::unique_ptr<std::thread>(
                new std::thread(
                        [fun, period, initial_delay, this]() {
                            if (initial_delay > 0) {
                                std::chrono::milliseconds sleep_duration(initial_delay);
                                std::this_thread::sleep_for(sleep_duration);
                            }

                            this->run(fun, period);
                        })
        );
    }

    void stop() {
        cpoint_.cancel();
        if (thread_ && thread_->joinable()) {
            thread_->join();
        }
		thread_.reset(nullptr);
    }

private:
    template<typename Function>
    void run(Function&& fun, long period) {
        try {
            while (true) {
                fun();
                if (period == 0) {
                    return;
                }
                cpoint_.wait(std::chrono::milliseconds(period));
            }
        } catch (...) {
            BOOST_LOG_TRIVIAL(debug) << "thread canelled";
        }
    }

    std::unique_ptr<std::thread> thread_;
    cancellation_point cpoint_;
};