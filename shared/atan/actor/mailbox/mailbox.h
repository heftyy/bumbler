#pragma once

#include <mutex>
#include <boost/any.hpp>
#include "../../messages/message.h"

class mailbox {
public:
    virtual void push_message(std::unique_ptr<message> msg) = 0;
    virtual std::unique_ptr<message> pop_message() = 0;
    virtual void clear() = 0;
    virtual bool empty() = 0;
    virtual unsigned long size() = 0;

    mailbox() { }

    mailbox(const mailbox& rhs) = delete;

	virtual ~mailbox() { }

    virtual std::unique_ptr<mailbox> clone() {

    }

protected:
    std::mutex mailbox_mutex_;

private:

};

