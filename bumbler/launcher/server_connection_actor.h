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
	server_connection_actor(std::string name, std::string remote_actor_ref, std::shared_ptr<actor_system> actor_system,
			std::function<void(std::string)> update_server_function)
		: remote_actor(name, actor_system, actor_ref(remote_actor_ref)), update_server_function_(update_server_function)
	{
		//actor_system_.add_actor(std::shared_ptr<test_actor>(this));
	}
	~server_connection_actor()
	{
		BOOST_LOG_TRIVIAL(debug) << "[SERVER_CONNECTION_ACTOR] destructor";
	}

private:

	std::function<void(std::string)> update_server_function_;

	void on_receive(message msg)
	{
        BOOST_LOG_TRIVIAL(debug) << "[SERVER_CONNECTION_ACTOR] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[SERVER_CONNECTION_ACTOR] received message from " << msg.sender.actor_name;
        BOOST_LOG_TRIVIAL(debug) << "message was " << std::string(msg.data.begin(), msg.data.end());

		if (msg.type == SERVER_STATUS)
		{
            BOOST_LOG_TRIVIAL(debug) << "[SERVER_CONNECTION_ACTOR] SERVER STATUS RECEIVED";
			update_server_function_(get_self().ip);
		}

		std::string msg_string = std::string(msg.data.begin(), msg.data.end());
		//reply(msg_string, msg.sender);
	}
};

