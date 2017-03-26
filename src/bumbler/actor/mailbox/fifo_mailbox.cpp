#include "fifo_mailbox.h"

namespace bumbler {

std::unique_ptr<bumbler::message> fifo_mailbox::pop_message() {
    std::unique_lock<std::mutex> lock(this->mailbox_mutex_);

    auto msg = std::move(this->queue_.front());
    this->queue_.pop_front();

    return std::move(msg);
}

std::vector<std::unique_ptr<bumbler::message>> fifo_mailbox::pop_messages(size_t count) {
    std::unique_lock<std::mutex> lock(this->mailbox_mutex_);

    std::vector<std::unique_ptr<message>> result;
    result.reserve(count);
    for (size_t i = 0; i < count; i++) {
        if (this->queue_.empty()) break;

        auto msg = std::move(this->queue_.front());
        this->queue_.pop_front();

        result.push_back(std::move(msg));
    }

    return result;
}

}