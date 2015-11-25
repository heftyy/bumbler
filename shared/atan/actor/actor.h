#pragma once

#include <chrono>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <future>
#include <memory>
#include <type_traits>
#include <logger/logger.h>
#include <utility.h>
#include <boost/static_assert.hpp>
#include "actor_ref/actor_ref.h"
#include "../actor_system/actor_system_errors.h"
#include "../messages/message.h"
#include "../messages/commands/commands.h"
#include "../packet/packet.h"
#include "untyped_actor.h"
#include "mailbox/fifo_mailbox.h"

class actor_system;

class actor {
public:
    actor(const std::string name, const std::shared_ptr<actor_system>& actor_system);

    actor(actor&& rhs) = delete;
    actor(const actor& rhs) = delete;
    actor& operator=(actor&& rhs) = delete;
    actor& operator=(const actor& rhs) = delete;
    virtual ~actor();

    virtual void stop_actor(bool wait = false);

    void read_messages();
    void pass_message(std::unique_ptr<message> msg, bool remote = false);
    void add_message(std::unique_ptr<message> msg);

    bool is_busy() { return busy_; }
    unsigned long mailbox_size() { return this->mailbox_->size(); }

    std::string actor_name();
    std::string system_name();

    actor_ref& get_self() {
        return self_;
    }

    void set_mailbox(std::unique_ptr<mailbox> mbox) {
        this->mailbox_ = std::move(mbox);
    }

protected:
    std::mutex actor_thread_mutex_;
    std::future<void> queue_thread_future_;
    std::unique_ptr<untyped_actor> untyped_actor_;
    std::unique_ptr<mailbox> mailbox_;
    std::atomic<bool> busy_;
    std::atomic<bool> stop_flag_;
    std::string actor_name_;
    std::weak_ptr<actor_system> actor_system_;
    std::condition_variable cv_;
    actor_ref self_;

    virtual void init(std::unique_ptr<untyped_actor> u_actor);

    virtual void tell(std::unique_ptr<message> msg, bool remote = false) = 0;

    virtual void on_receive(boost::any data) {
        this->untyped_actor_->on_receive(data);
    }

    virtual void on_error(boost::any data, std::exception& ex) {
        this->untyped_actor_->on_error(data, ex);
    }

    void create_internal_queue_thread();

    static void add_to_actor_system(const std::shared_ptr<actor_system>& system, std::unique_ptr<actor> actor_ptr);

private:
    void run_task(const actor_ref& sender, const boost::any& data) {
        busy_.store(true);
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] starting new task";
        this->untyped_actor_->set_sender(sender);

        try {
            on_receive(data);
        }
        catch(std::exception ex) {
            on_error(data, ex);
        }

        this->untyped_actor_->set_sender(actor_ref::none());
        busy_.store(false);
    }

    void clear_queue() {
        if(this->mailbox_) this->mailbox_->clear();
    }

    void send_reply_message(std::unique_ptr<message> msg);

};
