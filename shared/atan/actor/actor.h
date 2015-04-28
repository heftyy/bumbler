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
	std::mutex task_mutex_;
	std::mutex queue_mutex_;
	std::unique_ptr<interruptible_thread> queue_thread_;
	std::queue<message> message_queue_;
	std::atomic<int> actor_sleep_ms_;
	std::atomic<bool> busy_;
	std::atomic<bool> stopped_;
	std::string actor_name_;
	std::weak_ptr<actor_system> actor_system_;

	actor(const std::string name, std::weak_ptr<actor_system> actor_system, int actor_sleep_ms = 10);
	~actor();

	virtual void tell(const message& msg) {}
	virtual void on_receive(message msg) {}

	std::string actor_name();
	std::string system_name();
	void reply(int type, std::string msg, actor_ref& target_ref);
	bool is_busy() { return busy_; }

	void add_message(const message msg)
	{
		std::lock_guard<std::mutex> guard(queue_mutex_);
		message_queue_.push(msg);
	}

	bool compare(std::shared_ptr<actor> actor)
	{
		return this->actor_name().compare(actor->actor_name()) == 0;
	}

	actor_ref get_self()
	{
		return actor_ref(actor_name(), system_name());
	}

	packet message_to_packet(message& msg)
	{
		packet_header header;
		header.type = PACKET_DATA;

		packet_data data(msg);
		packet p(header, data);

		return p;
	}

	void read_messages()
	{
		std::unique_ptr<message> msg = get_first_message();

		if (msg)
		{
			run_task(std::move(msg));
			remove_finished_message();
		}
	}

	std::unique_ptr<message> get_first_message()
	{
		std::lock_guard<std::mutex> guard(queue_mutex_);
		if (message_queue_.size() > 0)
		{
			std::unique_ptr<message> msg = std::unique_ptr<message>(new message(message_queue_.front()));
			return msg;
		}
		return nullptr;
	}

	void remove_finished_message()
	{
		std::lock_guard<std::mutex> guard(queue_mutex_);
		if (message_queue_.size() > 0) message_queue_.pop();
	}

	void run_task(std::unique_ptr<message> msg)
	{
		busy_.store(true);
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] queueing new task";
		std::lock_guard<std::mutex> guard(task_mutex_);
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] starting new task";
		on_receive(*msg.release());
		busy_.store(false);
	}

	/** synchronous wait for the actor to end all tasks and stop all thread
	*/
	void stop_actor()
	{
		while (1)
		{
			if (queue_thread_ && get_first_message() == nullptr)
			{
                BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopping actor";
				queue_thread_->stop();
				queue_thread_.release();
				break;
			}
			else if (stopped_)
			{
				break;
			}
			else
			{
				std::chrono::milliseconds sleep_duration(actor_sleep_ms_);
				std::this_thread::sleep_for(sleep_duration);
			}
		}
	}
};
