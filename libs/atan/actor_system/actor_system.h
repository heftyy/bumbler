#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
#include "udp_server.h"
#include "actor_system_errors.h"
#include "../message.h"
#include "../actor/actor.h"
#include "../packet/packet.h"

class actor_system
{
public:
	friend class actor;
	/*actor_system(const std::string& name, boost::asio::io_service& io_service, int port) : system_name_(name), port_(port), io_service_(io_service)*/
	actor_system(const std::string& name, int port) : system_name_(name), port_(port)
	{
		stopped_.store(false);
		boost::asio::io_service::work work(io_service_);
		server_ = std::shared_ptr<udp_server>(new udp_server(io_service_, port_));
		server_->send_completed_function = [&](){ send_completed(); };
		server_->receive_function = [&](std::unique_ptr<packet> packet, boost::asio::ip::udp::endpoint sender_endpoint) { receive(std::move(packet), sender_endpoint); };
		io_service_thread_ = std::unique_ptr<std::thread>(new std::thread([this]()
		{
			std::cout << "[ACTOR_SYSTEM] STARTED ON PORT " << port_ << std::endl;
			io_service_.run();
		}));
	}

	~actor_system()
	{
		stop();
	}

	void stop()
	{
		if (stopped_.load()) return;
		std::cout << "[ACTOR_SYSTEM] SHUTTING DOWN" << std::endl;
		stopped_.store(true);
		server_->stop();
		if (!io_service_.stopped()) io_service_.stop();
		if (io_service_thread_->joinable()) io_service_thread_->join();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		server_.reset();
	}

	template<class actor_type>
	int add_actor(std::shared_ptr<actor_type>& actor)
	{
		if (stopped_) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);
		std::lock_guard<std::mutex> guard(actors_mutex_);
		for (auto iter_actor : actors_)
		{
			if (actor->compare(iter_actor))
			{
				return atan_error(ATAN_ACTOR_ALREADY_EXISTS, iter_actor->actor_name());
			}
		}
		actors_.push_back(actor);
		return 0;
	}

	int remove_actor(std::shared_ptr<actor> actor)
	{
		if (stopped_) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);
		std::lock_guard<std::mutex> guard(actors_mutex_);
		for (auto iter = actors_.begin(); iter != actors_.end(); iter++)
		{
			std::shared_ptr<class actor> iter_actor = *iter;
			if (actor->compare(iter_actor))
			{
				actors_.erase(iter);
				return 0;
			}
		}
		return atan_error(ATAN_ACTOR_NOT_FOUND, actor->actor_name());
		//throw new actor_not_found(actor->actor_name());
	}

	int tell_actor(message& message)
	{
		if (stopped_) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);
		if (message.target.system_name.compare(system_name_) != 0) return atan_error(ATAN_WRONG_ACTOR_SYSTEM, system_name_);
		//if (message.target.system_name.compare(system_name_) != 0) throw new wrong_actor_system(system_name_);
		std::lock_guard<std::mutex> guard(actors_mutex_);
		for (auto actor : actors_)
		{
			if (message.target.actor_name.compare(actor->actor_name()) == 0)
			{
				actor->add_message(message);
				return 0;
			}
		}
		return atan_error(ATAN_ACTOR_NOT_FOUND, message.target.actor_name);
		//throw new actor_not_found(message.target.actor_name);
	}

	std::shared_ptr<actor> get_actor(std::string actor_name)
	{
		if (stopped_) nullptr;
		std::lock_guard<std::mutex> guard(actors_mutex_);
		for (auto actor : actors_)
		{
			//std::shared_ptr<class actor> iter_actor = *iter;
			if (actor_name.compare(actor->actor_name()) == 0)
			{
				return actor;
			}
		}
		return nullptr;
	}

	std::shared_ptr<udp_server> server()
	{
		return server_;
	}

	std::string system_name()
	{
		return system_name_;
	}

private:
	int port_;
	std::atomic<bool> stopped_;
	std::string system_name_;
	std::vector<std::shared_ptr<actor>> actors_;
	std::mutex actors_mutex_;
	std::shared_ptr<udp_server> server_;
	std::unique_ptr<std::thread> io_service_thread_;
	boost::asio::io_service io_service_;

	void send_completed()
	{
		//std::cout << "SEND COMPLETED" << std::endl;
	}

	void receive(std::unique_ptr<packet> packet, boost::asio::ip::udp::endpoint& sender_endpoint)
	{
		message msg;
		message::restore_message(msg, packet->data.data);
		msg.sender.ip = sender_endpoint.address().to_string();
		msg.sender.port = sender_endpoint.port();

		if (msg.target.exists())
		{
			try
			{
				tell_actor(msg);
			}
			catch (std::runtime_error& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
		return;

	}
};
