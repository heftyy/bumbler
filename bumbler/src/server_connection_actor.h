#pragma once

#include <iostream>
#include <atomic>
#include <atan/interruptible_thread.h>
#include <atan/actor/remote_actor.h>
#include <atan/actor_system/actor_system.h>
#include <communication/message_settings.h>

class server_connection_actor : public remote_actor
{
public:
	server_connection_actor(std::string name, std::string& remote_actor_ref, actor_system& actor_system, std::function<void(std::string)> update_server_function, int actor_sleep_ms = 10)
		: remote_actor(name, actor_system, actor_ref(remote_actor_ref), actor_sleep_ms), update_server_function_(update_server_function)
	{
		//actor_system_.add_actor(std::shared_ptr<test_actor>(this));
	}
	~server_connection_actor()
	{
		std::cout << "[SERVER_CONNECTION_ACTOR] destructor" << std::endl;
	}

private:

	std::function<void(std::string)> update_server_function_;

	void on_receive(message msg)
	{
		std::cout << "[SERVER_CONNECTION_ACTOR] on_receive thread id = " << std::this_thread::get_id() << std::endl;
		std::cout << "[SERVER_CONNECTION_ACTOR] received message from " << msg.sender.actor_name << std::endl;
		std::cout << "message was " << std::string(msg.data.begin(), msg.data.end()) << std::endl;

		if (msg.type == SERVER_STATUS)
		{
			std::cout << "[SERVER_CONNECTION_ACTOR] SERVER STATUS RECEIVED" << std::endl;
			update_server_function_(get_self().ip);
		}

		std::string msg_string = std::string(msg.data.begin(), msg.data.end());
		//reply(msg_string, msg.sender);
	}
};

