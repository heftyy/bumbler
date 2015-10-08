#pragma once

#include <mutex>
#include <boost/any.hpp>

template<typename T>
class mailbox {
public:
    virtual void push_message(T&& message) = 0;
    virtual void push_message(T& message) = 0;
    virtual T pop_message() = 0;
    virtual void clear() = 0;
    virtual bool empty() = 0;

    mailbox() { }

    mailbox(const mailbox& rhs) { }

	virtual ~mailbox() { }

protected:
    std::mutex mailbox_mutex_;

private:

};

