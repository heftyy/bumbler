#pragma once

#include <chrono>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <memory>
#include <type_traits>
#include <logger/logger.h>
#include <boost/static_assert.hpp>
#include "actor_ref.h"
#include "../actor_system/actor_system_errors.h"
#include "../messages/message.h"
#include "../interruptible_thread.h"
#include "../packet/packet.h"
#include "../../utility.h"

class actor_system;
class router;

class actor : public std::enable_shared_from_this<actor> {
public:
    friend class actor_system;
    friend class router;
    friend class random_router;
    friend class round_robin_router;

    template<class T, typename ...Args>
    static actor_ref create_actor(Args&& ...args) {
        BOOST_STATIC_ASSERT_MSG(
                !(std::is_base_of<router, T>::value),
                "T cannot be a descendant of router"
        );

        std::shared_ptr<T> actor = std::shared_ptr<T>(new T(args...));
        actor->init();
        return actor->get_self();
    }

    template<class T, typename ...Args>
    static actor_ref create_router(Args&& ...args) {
        BOOST_STATIC_ASSERT_MSG(
                (std::is_base_of<router, T>::value),
                "T has be a descendant of router"
        );

        std::shared_ptr<T> router = std::shared_ptr<T>(new T(args...));
        router->init();
        router->template create_actors<T>();
        return router->get_self();
    }

    ~actor();

protected:
    std::mutex mutex_;
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

    std::string actor_name();

    std::string system_name();

    bool compare(std::shared_ptr<actor> actor);

    void pass_message(std::unique_ptr<message> msg, bool remote = false);

    void read_messages() {
        while (message_queue_.size() > 0) {
            std::unique_ptr<message> msg = std::move(message_queue_.front());
            run_task(msg->get_sender(), msg->get_data());
            message_queue_.pop();
        }
    }

    void reply(const char* data) {
        reply(std::string(data));
    }

    template<typename T>
    void reply(T data) {
        if (stop_flag_.load()) return;
        std::unique_ptr<typed_message<T>> typed_msg = std::unique_ptr<typed_message<T>>(new typed_message<T>());
        typed_msg->data = data;

        typed_msg->set_sender(get_self());
        typed_msg->set_target(get_sender());

        send_reply_message(std::move(typed_msg));
    }

    bool is_busy() { return busy_; }

    void add_message(std::unique_ptr<message> msg) {
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] queueing new task";
        std::unique_lock<std::mutex> lock(this->mutex_);
        message_queue_.push(std::move(msg));
        cv.notify_all();
    }

    void run_task(actor_ref& sender, boost::any data) {
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

    actor_ref& get_self() {
        return self_;
    }

    actor_ref& get_sender() {
        return sender_;
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
    void clear_queue() {
        std::unique_lock<std::mutex> lock(this->mutex_);
        while (!this->message_queue_.empty())
            this->message_queue_.pop();  // empty the queue
    }

    /** synchronous wait for the actor to end all tasks and stop all thread */
    virtual void stop_actor(bool wait = false) {
        if(stop_flag_) {
            return;
        }

        if (!wait)
            clear_queue();

        if (queue_thread_) {
            BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopping";
            stop_flag_.store(true);
            cv.notify_all();
            queue_thread_->stop();
            queue_thread_.release();
            BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopped";
        }
    }

    void send_reply_message(std::unique_ptr<message> msg);

};
