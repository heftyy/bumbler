#pragma once

#include <queue>
#include "standard_mailbox.h"

namespace bumbler {

class fifo_mailbox : public standard_mailbox {
public:
    fifo_mailbox() : standard_mailbox() {}

    fifo_mailbox(fifo_mailbox&&) = default; // support moving
    fifo_mailbox& operator=(fifo_mailbox&&) = default;

    std::unique_ptr<message> pop_message() override;

    std::vector<std::unique_ptr<message>> pop_messages(size_t count) override;

    std::unique_ptr<mailbox> clone() const override {
        return std::unique_ptr<mailbox>(new fifo_mailbox(*this));
    }

protected:
    fifo_mailbox(const fifo_mailbox& rhs) : standard_mailbox(rhs) { }
    fifo_mailbox& operator=(const fifo_mailbox& rhs) {
        if(this != &rhs) {
            standard_mailbox::operator=(rhs);
        }
        return *this;
    }
};

}
