#pragma once

#include <queue>
#include <logger/logger.h>
#include "mailbox.h"

template<typename T>
struct priority_message {
    T message;
    int priority;

    priority_message(T&& message, int priority) : message(std::forward<T>(message)), priority(priority) { }
    priority_message(T& message, int priority) : message(std::move(message)), priority(priority) { }

    priority_message(priority_message<T>& p_msg) = delete;

    priority_message(priority_message<T>&& p_msg) {
        this->message = std::move(p_msg.message);
        this->priority = p_msg.priority;
    }

    priority_message& operator=(priority_message<T>& p_msg) = delete;

    priority_message& operator=(priority_message<T>&& p_msg) {
        this->message = std::move(p_msg.message);
        this->priority = p_msg.priority;
    }
};

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

