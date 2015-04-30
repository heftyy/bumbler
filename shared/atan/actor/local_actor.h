#pragma once

#include "actor.h"
#include "../actor_system/actor_system_errors.h"

class local_actor : public actor
{
public:

    actor_ref init();

protected:
	local_actor(const std::string& name, std::shared_ptr<actor_system> actor_system)
		: actor(name, actor_system) {}

	void tell(const message& msg)
	{
		add_message(msg);
	}

	void tell(int type, std::string msg = "")
	{
		message message = string_to_message(type, msg);
		add_message(message);
		//atan_error(ATAN_WRONG_ACTOR_METHOD, "network tell in local_actor");
	}

	message string_to_message(int message_type, std::string msg)
	{
		message message_object = message(get_self(), get_self(), msg, message_type);
		return message_object;
	}
};
