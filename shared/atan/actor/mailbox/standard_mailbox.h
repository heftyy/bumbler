#pragma once

#include <queue>
#include <logger/logger.h>
#include "mailbox.h"

template<typename T>
class standard_mailbox : public mailbox<T> {

public:
    void push_message(T&& message) override {
        this->push_any_message(std::move(message));
    }

    void push_message(T& message) override {
        this->push_any_message(message);
    }

    T pop_message() override {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);

        auto msg = std::move(this->queue_.front());
        this->queue_.pop();

        return std::move(msg);
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
    template<typename _Tp>
    void push_any_message(_Tp&& message) {
        BOOST_LOG_TRIVIAL(debug) << boost::typeindex::type_id_with_cvr<T>().pretty_name();
        BOOST_LOG_TRIVIAL(debug) << boost::typeindex::type_id_with_cvr<decltype(message)>().pretty_name();

        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        this->queue_.push(std::forward<T>(message));
    }

    std::queue<T> queue_;

};

