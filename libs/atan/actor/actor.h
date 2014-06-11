#pragma once

#include <chrono>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include "../message.h"
#include "../interruptible_thread.h"
#include "../packet/packet.h"

class actor_system;

class actor
{
public:
	actor(const std::string& name, actor_system& actor_system, int actor_sleep_ms = 10);
	//actor(const actor& actor, actor_system& actor_system) : actor_system_(actor_system) {}

	~actor();

	virtual void tell(const message& msg) {}
	virtual void tell(int type, std::string msg, std::shared_ptr<actor> sender_actor = nullptr) {}
	virtual void on_receive(message msg) {}
	std::string actor_name();
	std::string system_name();
	void reply(int type, std::string msg, actor_ref& target_ref);
	bool is_busy() { return busy_; }

	void init()
	{
		message_queue_ = std::queue<message>();
		queue_thread_ = std::unique_ptr<interruptible_thread>(new interruptible_thread());
		queue_thread_->start_thread([this]() {
			while (1)
			{
				if (stopped_) return;
				queue_thread_->check_for_interrupt();
				this->read_messages();
				std::chrono::milliseconds sleep_duration(actor_sleep_ms_);
				std::this_thread::sleep_for(sleep_duration);
			}
		});
	}

	void add_message(const message msg)
	{
		std::lock_guard<std::mutex> guard(queue_mutex_);
		message_queue_.push(msg);
	}

	bool compare(std::shared_ptr<actor> actor)
	{
		if (this->actor_name().compare(actor->actor_name()) == 0) return true;
		return false;
	}

	actor_ref get_self()
	{
		return actor_ref(actor_name(), system_name());
	}

protected:
	std::mutex task_mutex_;
	std::mutex queue_mutex_;
	std::unique_ptr<interruptible_thread> queue_thread_;
	std::queue<message> message_queue_;
	std::atomic<int> actor_sleep_ms_;
	std::atomic<bool> busy_;
	std::atomic<bool> stopped_;
	std::string actor_name_;
	actor_system& actor_system_;

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
		if (msg != nullptr)
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
		busy_ = true;
		std::cout << "[ACTOR] queueing new task" << std::endl;
		std::lock_guard<std::mutex> guard(task_mutex_);
		std::cout << "[ACTOR] starting new task" << std::endl;
		on_receive(*msg.release());
		busy_ = false;
	}

	/** synchronous wait for the actor to end all tasks and stop all thread
	*/
	void stop_actor()
	{
		while (1)
		{
			if (queue_thread_ && get_first_message() == nullptr)
			{
				std::cout << "[ACTOR] stopping actor" << std::endl;
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
