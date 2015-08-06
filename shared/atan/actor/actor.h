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

class actor_system;
class router;

class actor : public std::enable_shared_from_this<actor> {
public:
    template<class T, typename ...Args>
    static actor_ref create_actor(Args&& ...args) {
        BOOST_STATIC_ASSERT_MSG(
                !(std::is_base_of<router, T>::value),
                "T cannot be a descendant of router"
        );

        std::unique_ptr<T> actor = utility::make_unique<T>(args...);
        actor->init();
        actor_ref& ar = actor->get_self();
        std::shared_ptr<actor_system> system = actor->actor_system_.lock();
        actor::add_to_actor_system(system, std::move(actor));
        return ar;
    }

    template<class T, typename ...Args>
    static actor_ref create_router(Args&& ...args) {
        BOOST_STATIC_ASSERT_MSG(
                (std::is_base_of<router, T>::value),
                "T has be a descendant of router"
        );

        std::unique_ptr<T> router = utility::make_unique<T>(args...);
        router->init();
        router->template create_actors<T>();
        actor_ref& ar = router->get_self();
        std::shared_ptr<actor_system> system = router->actor_system_.lock();
        actor::add_to_actor_system(system, std::move(router));
        return ar;
    }

    virtual void stop_actor(bool wait = false);

    void run_task(const actor_ref& sender, const boost::any& data) {
        busy_.store(true);
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] starting new task";
        this->sender_ = sender;

        try {
            on_receive(data);
        }
        catch(std::exception ex) {
            on_error(data, ex);
        }

        this->sender_ = actor_ref::none();
        busy_.store(false);
    }

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

    actor_ref& get_sender() {
        return sender_;
    }

    virtual ~actor();

protected:
    std::mutex message_queue_mutex_;
    std::mutex actor_thread_mutex_;
    std::unique_ptr<interruptible_thread> queue_thread_;
    std::queue<std::unique_ptr<message>> message_queue_;
    std::atomic<bool> busy_;
    std::atomic<bool> stop_flag_;
    std::string actor_name_;
    std::weak_ptr<actor_system> actor_system_;
    std::condition_variable cv;
    actor_ref sender_;
    actor_ref self_;

    actor(const std::string name, std::weak_ptr<actor_system> actor_system);

    virtual actor_ref init();

    virtual void tell(std::unique_ptr<message> msg, bool remote = false) {
        atan_error(ATAN_WRONG_ACTOR_METHOD, "virtual tell called, should never happen");
    }

    virtual void on_receive(boost::any data) {
        atan_error(ATAN_WRONG_ACTOR_METHOD, "virtual on_receive called, should never happen");
    };

    virtual void on_error(boost::any data, std::exception ex) {
        atan_error(ATAN_WRONG_ACTOR_METHOD, "virtual on_error called, override if needed");
    };

    void create_internal_queue_thread();

    void reply(const char* data) {
        reply(std::string(data));
    }

    template<typename T>
    void reply(T data) {
        if (stop_flag_.load()) return;
        auto typed_msg = construct_reply_message(data);

        send_reply_message(std::move(typed_msg));
    }

    template<typename T>
    std::unique_ptr<typed_message<T>> construct_reply_message(T data) {
        auto typed_msg = std::unique_ptr<typed_message<T>>(new typed_message<T>(get_sender(), get_self(), data));
        return std::move(typed_msg);
    }

    template<typename T>
    std::unique_ptr<typed_message<T>> construct_reply_message(::broadcast<T> msg) {
        auto typed_msg = std::unique_ptr<typed_message<T>>(new typed_message<T>(get_sender(), get_self(), msg));
        return std::move(typed_msg);
    }

    template<typename T>
    std::unique_ptr<typed_message<T>> construct_reply_message(::stop_actor<T> msg) {
        auto typed_msg = std::unique_ptr<typed_message<T>>(new typed_message<T>(get_sender(), get_self(), msg));
        return std::move(typed_msg);
    }

    template<typename T>
    std::unique_ptr<typed_message<T>> construct_reply_message(::kill_actor<T> msg) {
        auto typed_msg = std::unique_ptr<typed_message<T>>(new typed_message<T>(get_sender(), get_self(), msg));
        return std::move(typed_msg);
    }

    template<typename T>
    T cast_message(boost::any& data) {
        if(data.type().hash_code() == typeid(T).hash_code()) {
            return boost::any_cast<T>(data);
        }
        else {
            throw std::runtime_error("Types mismatch in cast_message");
        }
    }

    template<typename T>
    bool is_type(boost::any& data) {
        return data.type().hash_code() == typeid(T).hash_code();
    }

    packet message_to_packet(std::unique_ptr<message> msg) {
        packet_header header;
        header.type = PACKET_DATA;

        packet_data data;
        data.load(std::move(msg));
        packet p(header, data);

        return p;
    }

private:
    static void add_to_actor_system(const std::shared_ptr<actor_system>& system, std::unique_ptr<actor> actor_ptr);

    void clear_queue() {
        std::unique_lock<std::mutex> lock(this->message_queue_mutex_);
        while (!this->message_queue_.empty())
            this->message_queue_.pop();  // empty the queue
    }

    void send_reply_message(std::unique_ptr<message> msg);

};
