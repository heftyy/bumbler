#pragma once

#include <queue>
#include "mailbox.h"

template<typename T>
class priority_mailbox : public mailbox<T> {

public:
    void push_message(T&& element) override {
        this->priority_queue_.emplace(std::move(element));
    }

    T&& pop_message() override {
        auto result = std::move(this->priority_queue_.top());
        this->priority_queue_.pop();
        return std::move(result);
    }

    void clear() {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        while(!empty()) {
            this->priority_queue_.pop();
        }
    }

    bool empty() {
        return this->priority_queue_.empty();
    }

private:
    std::priority_queue<T> priority_queue_;
};

