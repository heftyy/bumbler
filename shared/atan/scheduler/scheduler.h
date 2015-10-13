#pragma once

#include <map>
#include <mutex>
#include <memory>
#include <iostream>
#include <atan/dispatcher/dispatcher.h>
#include "cancellable.h"
#include "../actor/actor_ref/actor_ref.h"
#include "utility.h"

class actor_system;

class scheduler {
public:
    scheduler(const std::shared_ptr<dispatcher>& dispatcher) : dispatcher_(dispatcher) {
//        cancellable_vector_it_ = cancellable_vector_.begin();
    }

    ~scheduler() {
        BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] DESTROY";

		/*
        for (std::shared_ptr<cancellable>& cancellable : cancellable_vector_) {
            if (!cancellable->is_cancelled()) {
                cancellable->cancel();
            }
        }
		*/
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule(const typed_message<T>& msg, long initial_delay_ms, long interval_ms) {
	    auto ret_cancellable = std::make_shared<cancellable>();
		std::weak_ptr<cancellable> ret_cancellable_weak_ptr = ret_cancellable;

        dispatcher_->push([this, initial_delay_ms, interval_ms](const typed_message<T> msg_copy, std::weak_ptr<cancellable> cancellable) -> int {
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
        }, std::move(msg), ret_cancellable_weak_ptr);

        return ret_cancellable;
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule_once(const typed_message<T>& msg, long initial_delay_ms) {
        return schedule<T>(msg, initial_delay_ms, 0);
    }

private:
	std::mutex cancellable_vector_mutex_;
    std::shared_ptr<dispatcher> dispatcher_;

};