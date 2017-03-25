#include "scheduler.h"

#include <map>
#include <mutex>
#include <iostream>

#include "cancellable.h"
#include "../messages/message.h"
#include "../actor_system/actor_system.h"

namespace bumbler {

std::shared_ptr<cancellable> scheduler::schedule(std::unique_ptr<message> msg, long initial_delay_ms, long interval_ms) const {
    auto ret_cancellable = std::make_shared<cancellable>();
    std::weak_ptr<cancellable> ret_cancellable_weak_ptr = ret_cancellable;

    std::thread scheduler_thread([this, initial_delay_ms, interval_ms](std::unique_ptr<message> moved_message, std::weak_ptr<cancellable> cancellable) -> int {
        if (cancellable.expired()) return 2;
        else {
            if (initial_delay_ms > 0) {
                std::chrono::milliseconds initial_delay(initial_delay_ms);
                std::this_thread::sleep_for(initial_delay);
            }
        }

        if (cancellable.expired()) return 2;
        else if (cancellable.lock()->check_cancel()) {
            cancellable.lock()->cancelled();
            return 1;
        };

        while (!cancellable.expired() && cancellable.lock() && !cancellable.lock()->check_cancel()) {
            BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] thread_id: " << std::this_thread::get_id() <<
                " sending message to " << moved_message->get_target().to_string();

            send_message(moved_message->clone());

            if (interval_ms > 0) {
                std::chrono::milliseconds interval(interval_ms);
                std::this_thread::sleep_for(interval);
            }
            else {
                break;
            }
        }

        if (cancellable.expired()) return 2;
        else {
            cancellable.lock()->cancelled();
            return 0;
        }
    }, std::move(msg), ret_cancellable_weak_ptr);

    scheduler_thread.detach();

    return ret_cancellable;
}

void scheduler::send_message(std::unique_ptr<message> msg) const {
    if (system_.expired())
        return;

    system_.lock()->tell_actor(std::move(msg));
}

}
