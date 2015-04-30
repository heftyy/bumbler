#include "actor.h"
#include "../actor_system/actor_system.h"

actor::actor(const std::string name, std::weak_ptr<actor_system> actor_system)
	: actor_name_(name), actor_system_(actor_system)
{
	stop_flag_.store(false);
}

actor::~actor()
{
    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] destructor";
	this->stop_actor();
}

std::string actor::actor_name()
{
	return this->actor_name_;
}

std::string actor::system_name()
{
	return actor_system_.lock()->system_name_;
}

template<typename T>
void actor::reply(T data, actor_ref sender)
{
	if (stop_flag_.load()) return;
    BOOST_LOG_TRIVIAL(debug) << "reply";
	message<T> message;
	message.type = 1;
	message.data = data;
	message.sender = get_self();
	message.target = get_sender();
	if (actor_system_.lock()->tell_actor(message) != 0 && message.target.valid_address())
	{
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] replied to " << message.target.to_string();
		packet p = message_to_packet(message);
		auto remote_actor_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address().from_string(message.target.ip), message.target.port);
		actor_system_.lock()->get_server()->do_send(p.get_raw_packet(), remote_actor_endpoint);
	}
}