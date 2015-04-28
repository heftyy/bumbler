#pragma once
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


class cancellation_point
{
public:
    cancellation_point(): stop_(false) {}

    void cancel() {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
        cond_.notify_all();
    }

    template <typename P>
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


class interruptible_thread
{
public:
    interruptible_thread() {}
    ~interruptible_thread()
    {
        stop();
    }

    template <typename Function, typename P>
    void start(Function&& fun, const P& period)
    {
        thread_ = std::unique_ptr<std::thread>(
                new std::thread(
                        [fun, period, this]() {
                            this->run(fun, period);
                        })
        );
    }

    void stop()
    {
        cpoint_.cancel();
        if(thread_ && thread_->joinable())
        {
            thread_->join();
        }
    }

private:
    template <typename Function, typename P>
    void run(Function&& fun, const P& period)
    {
        std::cout << "thread started\n";
        try {
            while (true) {
                fun();
                if(period == std::chrono::milliseconds(0))
                {
                    break;
                }
                cpoint_.wait(period);
            }
        } catch (const interrupt_thread_error&) {
            BOOST_LOG_TRIVIAL(debug) << "thread canelled";
        }
    }

    std::unique_ptr<std::thread> thread_;
    cancellation_point cpoint_;
};