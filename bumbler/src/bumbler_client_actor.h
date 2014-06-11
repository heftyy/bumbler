#pragma once

#include <iostream>
#include <atomic>
#include <atan/interruptible_thread.h>
#include <atan/actor/remote_actor.h>
#include <atan/actor_system/actor_system.h>

class bumbler_client_actor : public remote_actor
{
public:
	bumbler_client_actor(std::string& remote_actor_ref, actor_system& actor_system, int actor_sleep_ms = 10)
		: remote_actor("bumbler_client_actor", actor_system, actor_ref(remote_actor_ref), actor_sleep_ms)
	{
		//actor_system_.add_actor(std::shared_ptr<test_actor>(this));
	}
	~bumbler_client_actor()
	{
		std::cout << "bumbler_client_actor destructor" << std::endl;
	}

private:

	void on_receive(message msg)
	{
		std::cout << "[bumbler_client_actor] on_receive thread id = " << std::this_thread::get_id() << std::endl;
		std::cout << "bumbler_client_actor received message from " << msg.sender.actor_name << std::endl;
		std::cout << "message was " << std::string(msg.data.begin(), msg.data.end()) << std::endl;

		if (msg.type == 2)
		{
			std::cout << "message type was 2, its all good!" << std::endl;
		}

		std::string msg_string = std::string(msg.data.begin(), msg.data.end());
		//reply(msg_string, msg.sender);
	}
};