#pragma once

#include <iostream>
#include <atomic>
#include <atan/actor/local_actor.h>
#include <atan/actor_system/actor_system.h>
#include <communication/message_settings.h>

class out_actor : public local_actor
{
public:
    out_actor(std::string name, std::shared_ptr<actor_system> actor_system, int actor_sleep_ms = 10)
		: local_actor(name, actor_system, actor_sleep_ms) {}

	~out_actor()
	{
		std::cout << "[OUT_ACTOR] destructor" << std::endl;
	}

private:

	void on_receive(message msg)
	{
		std::cout << "[SERVER_CONNECTION_ACTOR] on_receive thread id = " << std::this_thread::get_id() << std::endl;
		std::cout << "[SERVER_CONNECTION_ACTOR] received message from " << msg.sender.actor_name << std::endl;
		std::cout << "message was " << std::string(msg.data.begin(), msg.data.end()) << std::endl;

		std::string msg_string = std::string(msg.data.begin(), msg.data.end());
		//reply(msg_string, msg.sender);
	}
};

