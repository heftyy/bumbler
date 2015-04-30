#pragma once

#include <chrono>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <memory>
#include <logger/logger.h>
#include "actor_ref.h"
#include "../message.h"
#include "../interruptible_thread.h"
#include "../packet/packet.h"

class actor_system;

class actor : public std::enable_shared_from_this<actor>
{
public:
	friend class actor_system;

    template<class T, typename ...Args>
    static actor_ref create_actor(Args&& ...args)
    {
        std::shared_ptr<T> actor = std::shared_ptr<T>(new T(args...));
        actor->init();
        return actor->get_self();
    }

    virtual actor_ref init() {}

protected:
	std::mutex mutex_;
	std::unique_ptr<interruptible_thread> queue_thread_;
	std::queue<message> message_queue_;
	std::atomic<bool> busy_;
	std::atomic<bool> stop_flag_;
	std::string actor_name_;
	std::weak_ptr<actor_system> actor_system_;
    std::condition_variable cv;

	actor(const std::string name, std::weak_ptr<actor_system> actor_system);
	~actor();

	virtual void tell(const message& msg) {}
	virtual void on_receive(message msg) {}

    packet message_to_packet(message& msg);
    std::string actor_name();
    std::string system_name();
	void reply(int type, std::string msg, actor_ref& target_ref);
	bool is_busy() { return busy_; }

	void add_message(const message msg)
	{
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] queueing new task";
        std::unique_lock<std::mutex> lock(this->mutex_);
		message_queue_.push(msg);
        cv.notify_all();
	}

	bool compare(std::shared_ptr<actor> actor)
	{
		return this->actor_name().compare(actor->actor_name()) == 0;
	}

	actor_ref get_self()
	{
		return actor_ref(actor_name(), system_name());
	}

	void read_messages()
	{
        while(message_queue_.size() > 0)
        {
            message msg = message_queue_.front();
            run_task(msg);
            message_queue_.pop();
        }
	}

	void run_task(message msg)
	{
		busy_.store(true);
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] starting new task";
		on_receive(msg);
		busy_.store(false);
	}

    void clear_queue() {
        std::unique_lock<std::mutex> lock(this->mutex_);
        while (!this->message_queue_.empty())
            this->message_queue_.pop();  // empty the queue
    }

	/** synchronous wait for the actor to end all tasks and stop all thread
	*/
    void stop_actor(bool wait = false)
    {
        if (!wait)
            clear_queue();

        if (queue_thread_)
        {
            BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopping";
            stop_flag_.store(true);
            cv.notify_all();
            queue_thread_->stop();
            queue_thread_.release();
            BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopped";
        }
    }
};
