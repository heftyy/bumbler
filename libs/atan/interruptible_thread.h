#pragma once
#include <mutex>
#include <thread>
#include <memory>
#include <stdexcept>
#include <string>

class interrupt_thread_error : public std::runtime_error {
public:
	interrupt_thread_error(const std::string& message) : std::runtime_error(message) { };
};

class interruptible_thread {
public:
	interruptible_thread() : _flag(false) {}

	//template <typename Function, typename... Args>
	//void start_thread(Function&& fun, Args&&... args) {
	template <typename Function>
	void start_thread(Function&& fun) {
		//thread_ = std::unique_ptr<std::thread>(new std::thread([](Function fun, Args... args)
		thread_ = std::unique_ptr<std::thread>(new std::thread([](Function fun)
		{
			try
			{
				//fun(std::forward<Args>(args)...);
				fun();
			}
			catch (std::runtime_error)
			{
			}
		}, std::forward<Function>(fun)));
		//}, std::forward<Function>(fun), std::forward<Args>(args)...));
	}

	~interruptible_thread()
	{
		join();
	}

	bool stopping() const
	{
		return _flag;
	}

	void stop()
	{
		_flag = true;
		join();
	}

	void join() {
		std::lock_guard<std::mutex> lock(_thread_mutex);
		if (thread_ && thread_->joinable())
		{
			thread_->join();
			std::cout << "[INTERRUPTIBLE_THREAD] thread joined: " << thread_->get_id() << std::endl;
		}
	}

	void check_for_interrupt() {
		if (!_flag.load()) { return; }

		throw interrupt_thread_error("thread interrupted");
	}

private:
	std::atomic<bool> _flag;
	std::unique_ptr<std::thread> thread_;
	std::mutex _thread_mutex;
};

