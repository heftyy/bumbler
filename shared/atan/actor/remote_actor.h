#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "actor.h"
#include "actor_ref.h"
#include "../packet/packet.h"

class remote_actor : public actor
{
public:

    actor_ref init();

protected:
	remote_actor(std::string name, std::shared_ptr<actor_system> actor_system, actor_ref network_actor_ref)
		: actor(name, actor_system), network_actor_ref_(network_actor_ref)
	{
		remote_actor_endpoint_ = boost::asio::ip::udp::endpoint(boost::asio::ip::address().from_string(network_actor_ref.ip), network_actor_ref.port);
	}

	void tell(message& msg)
	{
		tell_(msg);
		//throw new wrong_actor_method("wrong tell method for network actor");
	}

	message string_to_message(int message_type, std::string msg)
	{
		message message_object = message(network_actor_ref_, get_self(), msg, message_type);
		return message_object;
	}

	void tell(int message_type, std::string msg = "")
	{
		message message_object = string_to_message(message_type, msg);
		tell_(message_object); 
	}

	message future(int message_type, int timeout_ms = 1000, std::string msg = "")
	{
		message message_object = string_to_message(message_type, msg);
		return future_(message_object, timeout_ms);
	}

private:
	boost::asio::ip::udp::endpoint remote_actor_endpoint_;
	actor_ref network_actor_ref_;

	void tell_(message& msg);
	message future_(message& msg, int timeout_ms);
};