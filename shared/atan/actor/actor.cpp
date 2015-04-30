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

packet actor::message_to_packet(message& msg)
{
    packet_header header;
    header.type = PACKET_DATA;

    packet_data data(msg);
    packet p(header, data);

    return p;
}

void actor::reply(int type, std::string msg, actor_ref& target_ref)
{
	if (stop_flag_.load()) return;
    BOOST_LOG_TRIVIAL(debug) << "reply";
	message message;
	message.type = type;
	message.data = msg;
	message.sender = get_self();
	message.target = target_ref;
	if (actor_system_.lock()->tell_actor(message) != 0 && message.target.valid_address())
	{
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] replied to " << target_ref.to_string();
		packet p = message_to_packet(message);
		auto remote_actor_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address().from_string(message.target.ip), message.target.port);
		actor_system_.lock()->get_server()->do_send(p.get_raw_packet(), remote_actor_endpoint);
	}
}