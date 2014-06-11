#include "actor.h"
#include "../actor_system/actor_system.h"

actor::actor(const std::string& name, actor_system& actor_system, int actor_sleep_ms) 
	: actor_name_(name), actor_system_(actor_system), actor_sleep_ms_(actor_sleep_ms)
{
	stopped_ = false;
	init();
}

actor::~actor()
{
	std::cout << "[ACTOR] destructor" << std::endl;
	stopped_ = true;
	this->stop_actor();
}

std::string actor::actor_name()
{
	return this->actor_name_;
}

std::string actor::system_name()
{
	return actor_system_.system_name_;
}

void actor::reply(int type, std::string msg, actor_ref& target_ref)
{
	if (stopped_.load()) return;
	std::cout << "reply" << std::endl;
	message message;
	message.type = type;
	message.data = msg;
	message.sender = get_self();
	message.target = target_ref;
	if (actor_system_.tell_actor(message) != 0 && message.target.valid_address())
	{
		std::cout << "[ACTOR] replied to " << message.target.ip << ":" << message.target.port << std::endl;
		packet p = message_to_packet(message);
		auto remote_actor_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address().from_string(message.target.ip), message.target.port);
		actor_system_.server()->do_send(p.get_raw_packet(), remote_actor_endpoint);
	}
}