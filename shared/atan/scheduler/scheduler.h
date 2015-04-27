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
        for(auto& pair : this->threads_map_)
        {
            pair.second->stop();
        }
    }

    void schedule(const message& message, long initial_delay_ms, long interval_ms)
    {
        std::shared_ptr<interruptible_thread> thread = std::shared_ptr<interruptible_thread>(new interruptible_thread());

        thread->start_thread([this, initial_delay_ms, interval_ms, message, thread]() {

            this->add_thread(std::move(thread));

            std::chrono::milliseconds init_sleep_duration(initial_delay_ms);
            std::this_thread::sleep_for(init_sleep_duration);
            while (1)
            {
                thread->check_for_interrupt();

                BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] sending message to " << message.target.to_string();

                message.send();

                if(interval_ms == 0) break;

                std::chrono::milliseconds interval_sleep_duration(interval_ms);
                std::this_thread::sleep_for(interval_sleep_duration);
            }

            //this->remove_thread();
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
        std::thread::id thread_id = std::this_thread::get_id();
        this->threads_map_.insert(std::make_pair(thread_id, std::move(thread)));
    }

    void remove_thread()
    {
        this->threads_map_.erase(std::this_thread::get_id());
    }
};