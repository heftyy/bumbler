#pragma once

#include <memory>
#include <boost/any.hpp>

namespace bumbler {

class actor_ref;

class message {
public:
    message() {}
    virtual ~message() {}

    message(message&& msg) = default; // support moving
    message& operator=(message&& msg) = default;

    message(const message& msg) = default;
    message& operator=(const message& msg) = default;

    virtual boost::any get_data() const = 0;
    virtual const actor_ref& get_sender() const = 0;
    virtual const actor_ref& get_target() const = 0;

    virtual std::unique_ptr<message> clone() const = 0;

    virtual void set_sender(const actor_ref& sender) = 0;
    virtual void set_target(const actor_ref& target) = 0;

    virtual int get_priority() const = 0;

    virtual bool is_broadcast() const = 0;
    virtual bool is_stop_actor() const = 0;
    virtual bool is_kill_actor() const = 0;
    virtual bool is_priority_message() const = 0;
};

}
