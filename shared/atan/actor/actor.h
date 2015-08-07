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
#include "actor_ref.h"
#include "../actor_system/actor_system_errors.h"
#include "../messages/message.h"
#include "../messages/commands/commands.h"
#include "../interruptible_thread.h"
#include "../packet/packet.h"
#include "untyped_actor.h"

class actor_system;
class router;

class actor : public std::enable_shared_from_this<actor> {
public:
    virtual void stop_actor(bool wait = false);

    void pass_message(std::unique_ptr<message> msg, bool remote = false);

    void read_messages();

    void add_message(std::unique_ptr<message> msg) {
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] queueing new task";
        std::unique_lock<std::mutex> lock(this->message_queue_mutex_);
        message_queue_.push(std::move(msg));
        cv.notify_one();
    }

    bool is_busy() { return busy_; }

    std::string actor_name();
    std::string system_name();

    actor_ref& get_self() {
        return self_;
    }

    virtual ~actor();

protected:
    std::mutex message_queue_mutex_;
    std::mutex actor_thread_mutex_;
    std::unique_ptr<interruptible_thread> queue_thread_;
    std::unique_ptr<untyped_actor> untyped_actor_;
    std::queue<std::unique_ptr<message>> message_queue_;
    std::atomic<bool> busy_;
    std::atomic<bool> stop_flag_;
    std::string actor_name_;
    std::weak_ptr<actor_system> actor_system_;
    std::condition_variable cv;
    actor_ref self_;

    actor(const std::string& name, const std::shared_ptr<actor_system>& actor_system);

    virtual void init(std::unique_ptr<untyped_actor> u_actor);

    virtual void tell(std::unique_ptr<message> msg, bool remote = false) {
        atan_error(ATAN_WRONG_ACTOR_METHOD, "virtual tell called, should never happen");
    }

    virtual void on_receive(boost::any data) {
        this->untyped_actor_->on_receive(data);
    };

    virtual void on_error(boost::any data, std::exception& ex) {
        this->untyped_actor_->on_error(data, ex);
    };

    void create_internal_queue_thread();

    packet message_to_packet(std::unique_ptr<message> msg) {
        packet_header header;
        header.type = PACKET_DATA;

        packet_data data;
        data.load(std::move(msg));
        packet p(header, data);

        return p;
    }

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
        std::unique_lock<std::mutex> lock(this->message_queue_mutex_);
        while (!this->message_queue_.empty())
            this->message_queue_.pop();  // empty the queue
    }

    void send_reply_message(std::unique_ptr<message> msg);

};
