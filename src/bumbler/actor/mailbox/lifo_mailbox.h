#pragma once

#include <queue>
#include "standard_mailbox.h"

namespace bumbler {

class lifo_mailbox : public standard_mailbox {
public:
    lifo_mailbox() : standard_mailbox() {}

    lifo_mailbox(lifo_mailbox&&) = default; // support moving
    lifo_mailbox& operator=(lifo_mailbox&&) = default;

    std::unique_ptr<message> pop_message() override;

    std::vector<std::unique_ptr<message>> pop_messages(size_t count) override;

    std::unique_ptr<mailbox> clone() const override {
        return std::unique_ptr<mailbox>(new lifo_mailbox(*this));
    }

protected:
    lifo_mailbox(const lifo_mailbox& rhs) : standard_mailbox(rhs) { }
    lifo_mailbox& operator=(const lifo_mailbox& rhs) {
        if(this != &rhs) {
            standard_mailbox::operator=(rhs);
        }
        return *this;
    }
};

}
