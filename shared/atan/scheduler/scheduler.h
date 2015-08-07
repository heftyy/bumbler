#pragma once

#include <map>
#include <mutex>
#include <memory>
#include <iostream>
#include <atan/dispatcher/dispatcher.h>
#include "cancellable.h"
#include "../thread_pool/ctpl_stl.h"
#include "../actor/actor_ref.h"
#include "../interruptible_thread.h"
#include "utility.h"

class actor_system;

class scheduler {
public:
    scheduler(const std::shared_ptr<dispatcher>& dispatcher) : dispatcher_(dispatcher) {
        cancellable_vector_it_ = cancellable_vector_.begin();
    }

    ~scheduler() {
        BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] DESTROY";

        for (std::shared_ptr<cancellable>& cancellable : cancellable_vector_) {
            if (!cancellable->is_cancelled()) {
                cancellable->cancel();
            }
        }
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule(const typed_message<T>& msg, long initial_delay_ms, long interval_ms) {
        std::shared_ptr<cancellable> ret_cancellable = std::make_shared<cancellable>();
        auto cancel_it = cancellable_vector_.insert(cancellable_vector_it_, ret_cancellable);

        dispatcher_->push([this, msg, initial_delay_ms, interval_ms, ret_cancellable, cancel_it](int id) -> int {
            ret_cancellable->thread_id = std::this_thread::get_id();

            if (initial_delay_ms > 0) {
                std::chrono::milliseconds initial_delay(initial_delay_ms);
                std::this_thread::sleep_for(initial_delay);
            }

            if (ret_cancellable->check_cancel()) {
                ret_cancellable->cancelled();
                cancellable_vector_.erase(cancel_it);
                return 1;
            }

            while (!ret_cancellable->check_cancel()) {
                BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] thread_id: " << std::this_thread::get_id() <<
                                         " sending message to " << msg.get_target().to_string();
                BOOST_LOG_TRIVIAL(debug) << "hello from " << id << ' ' << '\n';

                msg.get_target().tell(msg);

                if (interval_ms > 0) {
                    std::chrono::milliseconds interval(interval_ms);
                    std::this_thread::sleep_for(interval);
                }
                else {
                    break;
                }
            }

            ret_cancellable->cancelled();
            cancellable_vector_.erase(cancel_it);

			return 0;
        });

        return ret_cancellable;
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule_once(const typed_message<T>& msg, long initial_delay_ms) {
        return schedule<T>(msg, initial_delay_ms, 0);
    }

private:
    std::shared_ptr<dispatcher> dispatcher_;
    std::vector<std::shared_ptr<cancellable>> cancellable_vector_;
    std::vector<std::shared_ptr<cancellable>>::iterator cancellable_vector_it_;

};