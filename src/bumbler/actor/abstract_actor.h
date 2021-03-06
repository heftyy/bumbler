#pragma once

#include <atomic>
#include <memory>
#include "actor_ref/actor_ref.h"
#include "../internal/bumbler.h"

namespace bumbler {

class actor_system;
class message;
class mailbox;
class untyped_actor;

class abstract_actor {
public:
    abstract_actor(const std::shared_ptr<actor_system>& actor_system, const std::string& name);

    abstract_actor(abstract_actor && rhs) = delete;
    abstract_actor(const abstract_actor & rhs) = delete;
    abstract_actor& operator=(abstract_actor && rhs) = delete;
    abstract_actor& operator=(const abstract_actor & rhs) = delete;
    virtual ~abstract_actor();

    virtual void init(std::unique_ptr<untyped_actor> u_actor);
    virtual void stop_actor(stop_mode stop_mode) = 0;

    virtual void tell(std::unique_ptr<message> msg) = 0;

    bool is_busy() const { return busy_; }
    size_t mailbox_size() const;

    identifier actor_key() const;
    identifier system_key() const;

    actor_ref get_self() const { return self_; }

    void set_mailbox(std::unique_ptr<mailbox> mailbox);

protected:
    std::unique_ptr<untyped_actor> untyped_actor_;
    std::unique_ptr<mailbox> mailbox_;
    std::atomic<bool> busy_;
    std::atomic<bool> stop_flag_;
    std::weak_ptr<actor_system> actor_system_;
    identifier actor_key_;
    actor_ref self_;

    virtual void on_receive(const boost::any& data);
    virtual void on_error(const boost::any& data, const std::exception& ex);

    void clear_queue();

};

}
