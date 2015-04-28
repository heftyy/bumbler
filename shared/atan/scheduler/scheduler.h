#pragma once

#include <map>
#include <mutex>
#include <memory>
#include <atan/actor_system/actor_system.h>
#include "../actor/actor_ref.h"
#include "../interruptible_thread.h"
#include "utility.h"

class scheduler
{
public:

    scheduler() {}

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

    void schedule(const message& message, long initial_delay_ms, long interval_ms)
    {
        std::thread([this, initial_delay_ms, interval_ms, &message]() {
            std::chrono::milliseconds init_sleep_duration(initial_delay_ms);
            std::this_thread::sleep_for(init_sleep_duration);

            std::shared_ptr<interruptible_thread> thread = std::shared_ptr<interruptible_thread>(new interruptible_thread());
            this->add_thread(thread);

            thread->start([&message]() {
                BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] thread_id: " << std::this_thread::get_id() << " sending message to " << message.target.to_string();

                message.send();
            }, std::chrono::milliseconds(interval_ms));
        });
    }

    void schedule_once(message& message, long initial_delay_ms)
    {
        schedule(message, initial_delay_ms, 0);
    }

private:

    std::map<std::thread::id, std::shared_ptr<interruptible_thread>> threads_map_;
    std::mutex threads_map_mutex_;

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