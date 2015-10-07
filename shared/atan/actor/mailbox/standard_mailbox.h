#pragma once

#include <queue>
#include <logger/logger.h>
#include "mailbox.h"

template<typename T>
class standard_mailbox : public mailbox<T> {

public:
    void push_message(T&& message) {
        BOOST_LOG_TRIVIAL(debug) << boost::typeindex::type_id_with_cvr<T>().pretty_name();
        BOOST_LOG_TRIVIAL(debug) << boost::typeindex::type_id_with_cvr<decltype(message)>().pretty_name();

        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        this->queue_.push(std::forward<T>(std::move(message)));
    }

    T&& pop_message() {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        auto msg = std::move(this->queue_.front());
        this->queue_.pop();
        return std::forward<T>(msg);
    }

    void clear() {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        while(!empty()) {
            this->queue_.pop();
        }
    }

    bool empty() {
        return this->queue_.empty();
    }

private:
    std::queue<T> queue_;

};

