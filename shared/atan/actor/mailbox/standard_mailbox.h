#pragma once

#include <queue>
#include <logger/logger.h>
#include "mailbox.h"

class standard_mailbox : public mailbox {

public:
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

    unsigned long size() override {
        return this->queue_.size();
    }

protected:
    std::queue<std::unique_ptr<message>> queue_;

private:
    void push_to_queue(std::unique_ptr<message> msg) {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
        this->queue_.emplace(std::move(msg));
    }

};

