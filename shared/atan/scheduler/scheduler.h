#pragma once

#include <map>
#include <mutex>
#include <memory>
#include "../thread_pool/ctpl_stl.h"
#include "../actor/actor_ref.h"
#include "../interruptible_thread.h"
#include "utility.h"

class scheduler
{
public:

    scheduler()
    {
        thread_pool_.resize(10);

        thread_pool_.push([](int id){  // lambda
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            std::cout << "hello from " << id << ' ' << '\n';
        });
    }

    ~scheduler()
    {
        BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] DESTROY";

        std::lock_guard<std::mutex> lock(threads_map_mutex_);
        for(auto& pair : this->threads_map_)
        {
            BOOST_LOG_TRIVIAL(debug) << "thread id: " << pair.first;
            pair.second->stop();
        }
    }

    void schedule(const message& message, long interval_ms, long initial_delay_ms)
    {
        std::shared_ptr<interruptible_thread> thread = std::shared_ptr<interruptible_thread>(new interruptible_thread());
        this->add_thread(thread);

        thread->start([&message]() {
            BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] thread_id: " << std::this_thread::get_id() << " sending message to " << message.target.to_string();

            message.send();
        }, interval_ms, initial_delay_ms);
    }

    void schedule_once(message& message, long initial_delay_ms)
    {
        schedule(message, 0, initial_delay_ms);
    }

private:

    std::map<std::thread::id, std::shared_ptr<interruptible_thread>> threads_map_;
    std::mutex threads_map_mutex_;
    ctpl::thread_pool thread_pool_;

    void add_thread(std::shared_ptr<interruptible_thread> thread)
    {
        std::lock_guard<std::mutex> lock(threads_map_mutex_);

        std::thread::id thread_id = std::this_thread::get_id();
        this->threads_map_.emplace(thread_id, std::move(thread));
    }

    void remove_thread()
    {
        std::lock_guard<std::mutex> lock(threads_map_mutex_);

        auto search = this->threads_map_.find(std::this_thread::get_id());
        if(search != this->threads_map_.end()) {
            this->threads_map_.erase(search);
        }
    }
};