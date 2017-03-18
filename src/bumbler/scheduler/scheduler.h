#pragma once

#include <map>
#include <mutex>
#include <memory>
#include <iostream>
#include "cancellable.h"
#include "../actor/actor_ref/actor_ref.h"

namespace bumbler {

class actor_system;

class scheduler {
public:
    scheduler() {
//        cancellable_vector_it_ = cancellable_vector_.begin();
    }

    ~scheduler() {
        /*
        for (std::shared_ptr<cancellable>& cancellable : cancellable_vector_) {
            if (!cancellable->is_cancelled()) {
                cancellable->cancel();
            }
        }
        */
    }

    std::shared_ptr<cancellable> schedule(std::unique_ptr<typed_message> msg, long initial_delay_ms, long interval_ms) const {
        auto ret_cancellable = std::make_shared<cancellable>();
        std::weak_ptr<cancellable> ret_cancellable_weak_ptr = ret_cancellable;

        std::thread scheduler_thread([initial_delay_ms, interval_ms](typed_message msg_copy, std::weak_ptr<cancellable> cancellable) -> int {
            if (cancellable.expired()) return 2;
            else {
                cancellable.lock()->thread_id = std::this_thread::get_id();

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

            while (!cancellable.expired() && cancellable.lock() != nullptr && !cancellable.lock()->check_cancel()) {
                BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] thread_id: " << std::this_thread::get_id() <<
                                         " sending message to " << msg_copy.get_target().to_string();

                msg_copy.get_target().tell(msg_copy);

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
        }, *msg, ret_cancellable_weak_ptr);

        scheduler_thread.detach();

        return ret_cancellable;
    }

};

}
