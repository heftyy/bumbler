#pragma once

#include <memory>
#include <queue>
#include "mailbox.h"

namespace bumbler {

class message_wrapper {
public:
    std::unique_ptr<message> msg;
    int priority;

    message_wrapper(std::unique_ptr<message> msg, int priority) : msg(std::move(msg)), priority(priority) { }
    message_wrapper(const message_wrapper& lhs) {
        this->msg = lhs.msg->clone();
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

    priority_mailbox() : mailbox() {}

    priority_mailbox(priority_mailbox&&) = default; // support moving
    priority_mailbox& operator=(priority_mailbox&&) = default;

    void push_message(std::unique_ptr<message> msg) {
        this->push_to_queue(std::move(msg), msg->get_priority());
    }

    std::unique_ptr<message> pop_message() override;

    std::vector<std::unique_ptr<message>> pop_messages(size_t count) override;

    void clear() override;

    bool empty() override {
        return this->queue_.empty();
    }

    size_t size() override {
        return this->queue_.size();
    }

    std::unique_ptr<mailbox> clone() const override {
        return std::unique_ptr<mailbox>(new priority_mailbox(*this));
    }

protected:
    priority_mailbox(const priority_mailbox& rhs) : mailbox(rhs) { }
    priority_mailbox& operator=(const priority_mailbox& rhs) {
        if(this != &rhs) {
            mailbox::operator=(rhs);
        }
        return *this;
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

}
