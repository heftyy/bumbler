#pragma once

#include <mutex>
#include "../../messages/message.h"

class mailbox {
public:
    virtual void push_message(std::unique_ptr<message> msg) = 0;
    virtual std::unique_ptr<message> pop_message() = 0;
    virtual void clear() = 0;
    virtual bool empty() = 0;
    virtual unsigned long size() = 0;

    mailbox() { }

    mailbox(mailbox&&) = default; // support moving
    mailbox& operator=(mailbox&&) = default;

    mailbox(const mailbox&) : mailbox_mutex_() { }
    mailbox& operator=(const mailbox& rhs) {
        if(this != &rhs) { }
        return *this;
    }

	virtual ~mailbox() { }
    virtual std::unique_ptr<mailbox> clone() const = 0;

protected:
    std::mutex mailbox_mutex_;
};

