#pragma once

#include <iostream>
#include <atomic>
#include <atan/interruptible_thread.h>
#include <atan/actor/local_actor.h>
#include <atan/actor_system/actor_system.h>

class server_actor: public local_actor
{
public:

	server_actor(actor_system& actor_system, int actor_sleep_ms = 10) : local_actor("server_actor", actor_system, actor_sleep_ms)
	{
	}

	~server_actor()
	{
	}

private:

	void on_receive(message msg)
	{
		std::cout << "[server_actor] on_receive thread id = " << std::this_thread::get_id() << std::endl;
		std::cout << "server_actor received message from " << msg.sender.actor_name << std::endl;
		std::cout << "message was " << std::string(msg.data.begin(), msg.data.end()) << std::endl;

		if (msg.type == 1)
		{
			reply(2, "HAHAHA", msg.sender);
		}
	}
};

