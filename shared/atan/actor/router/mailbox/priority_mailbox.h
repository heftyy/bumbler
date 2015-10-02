#pragma once

#include <queue>
#include "mailbox.h"

template<typename T>
class priority_mailbox : mailbox<T> {
    void push_message(T&& element) override {
        this->priority_queue_.emplace(std::move(element));
    }

    T&& pop_message() override {
        auto result = std::move(this->priority_queue_.top());
        this->priority_queue_.pop();
        return std::move(result);
    }

private:
    std::priority_queue<T> priority_queue_;
};

