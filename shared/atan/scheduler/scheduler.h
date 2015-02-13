#pragma once

#include "../actor/actor_ref.h"
#include <vector>
#include <atan/interruptible_thread.h>

class scheduler
{
public:

    scheduler() {}

    void schedule(actor_ref actor, message& message, long initial_delay_ms, long interval_ms)
    {
        auto thread = std::shared_ptr<interruptible_thread>(new interruptible_thread());
        threads_.push_back(thread);

        thread->start_thread([initial_delay_ms, interval_ms, message, &actor, &thread]() {
            std::chrono::milliseconds init_sleep_duration(initial_delay_ms);
            std::this_thread::sleep_for(init_sleep_duration);
            while (1)
            {
                thread->check_for_interrupt();

                std::cout << "Aaaaaaaaa" << std::endl;

                std::chrono::milliseconds interval_sleep_duration(interval_ms);
                std::this_thread::sleep_for(interval_sleep_duration);
            }
        });
    }

private:
    std::vector<std::shared_ptr<interruptible_thread>> threads_;
};