#pragma once

#include <queue>
#include <logger/logger.h>
#include "standard_mailbox.h"

class fifo_mailbox : public standard_mailbox {

public:
    std::unique_ptr<message> pop_message() override {
        std::unique_lock<std::mutex> lock(this->mailbox_mutex_);

        auto msg = std::move(this->queue_.front());
        this->queue_.pop();

        return std::move(msg);
    }
};

