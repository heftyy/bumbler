#pragma once

#include <queue>
#include <logger/logger.h>
#include "mailbox.h"
#include "../../messages/commands/priority_message.h"

template<typename T>
class priority_mailbox : public mailbox<T> {

public:
    void push_message(T&& message) override {
        this->push_any_message(std::move(message));
    }

    void push_message(T& message) override {
        this->push_any_message(message);
    }

    T pop_message() override {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);

        T moved = std::move(const_cast<T&>(this->queue_.top()));
        this->queue_.pop();

        return std::move(moved);
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
    using Queue = std::priority_queue<T, std::deque<T>, std::function<bool(const T&, const T&)>>;
    Queue queue_ = Queue([](const T& lhs, const T& rhs) {
        return lhs.priority < rhs.priority;
    });

    template<typename _Tp>
    void push_any_message(_Tp&& message) {
        BOOST_LOG_TRIVIAL(debug) << boost::typeindex::type_id_with_cvr<T>().pretty_name();
        BOOST_LOG_TRIVIAL(debug) << boost::typeindex::type_id_with_cvr<decltype(message)>().pretty_name();

        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        this->queue_.push(std::forward<T>(message));
    }

};

