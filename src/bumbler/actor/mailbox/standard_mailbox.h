#pragma once

#include <queue>
#include "mailbox.h"

namespace bumbler {

class standard_mailbox : public mailbox {
public:
    standard_mailbox() : mailbox() { }

    standard_mailbox(standard_mailbox&&) = default; // support moving
    standard_mailbox& operator=(standard_mailbox&&) = default;

    virtual ~standard_mailbox() { }

    void push_message(std::unique_ptr<message> message) override {
        this->push_to_queue(std::move(message));
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

    size_t size() override {
        return this->queue_.size();
    }

    virtual std::unique_ptr<mailbox> clone() const = 0;

protected:
    std::queue<std::unique_ptr<message>> queue_;

    standard_mailbox(const standard_mailbox& rhs) : mailbox(rhs), queue_() { }
    standard_mailbox& operator=(const standard_mailbox& rhs) {
        if(this != &rhs) {
            mailbox::operator=(rhs);
        }
        return *this;
    }

private:
    void push_to_queue(std::unique_ptr<message> msg) {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        this->queue_.emplace(std::move(msg));
    }

};

}
