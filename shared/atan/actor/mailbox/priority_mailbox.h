#pragma once

#include <memory>
#include <queue>
#include <boost/any.hpp>
#include <logger/logger.h>
#include "mailbox.h"

class message_wrapper {
public:
    std::unique_ptr<message> msg;
    int priority;

    message_wrapper(std::unique_ptr<message> msg, int priority) : msg(std::move(msg)), priority(priority) { }
    message_wrapper(const message_wrapper& lhs) {
        this->msg = lhs.msg->clone_ptr();
        this->priority = lhs.priority;
    }

    message_wrapper(message_wrapper&& lhs) {
        this->msg = std::move(lhs.msg);
        this->priority = lhs.priority;
    }

    message_wrapper& operator=(message_wrapper&& lhs) {
        this->msg = std::move(lhs.msg);
        this->priority = lhs.priority;
        return *this;
    }
};

class priority_mailbox : public mailbox {

public:
    using QueueElement = message_wrapper;
    using Queue = std::priority_queue<QueueElement, std::deque<QueueElement>, std::function<bool(const QueueElement&, const QueueElement&)>>;

    void push_message(std::unique_ptr<message> msg) {
        this->push_to_queue(std::move(msg), msg->get_priority());
    }

    std::unique_ptr<message> pop_message() override {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);

        QueueElement moved = std::move(const_cast<QueueElement&>(this->queue_.top()));
        this->queue_.pop();

        return std::move(moved.msg);
    }

    void clear() override {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        while(!empty()) {
            this->queue_.pop();
        }
    }

    bool empty() override {
        return this->queue_.empty();
    }

    unsigned long size() override {
        return this->queue_.size();
    }

private:
    Queue queue_ = Queue([](const QueueElement& lhs, const QueueElement& rhs) {
        return lhs.priority < rhs.priority;
    });

    void push_to_queue(std::unique_ptr<message> msg, int priority) {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        this->queue_.emplace(std::move(msg), priority);
    }

};
