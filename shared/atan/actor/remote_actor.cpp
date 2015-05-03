#include "remote_actor.h"
#include "../actor_system/actor_system.h"

actor_ref remote_actor::init() {
    this->actor_system_.lock()->add_actor(shared_from_this());
    return this->get_self();
}

void remote_actor::tell_(packet& p) {
    if (!actor_system_.lock()->get_server()) atan_error(ATAN_SERVER_DOESNT_EXIST, "server doesn't exist");

    actor_system_.lock()->get_server()->do_send(p.get_raw_packet(), remote_actor_endpoint_);
}

/*
template<typename T>
boost::any remote_actor::future_(message<T>& msg, int timeout_ms)
{
	if (!actor_system_.lock()->get_server()) atan_error(ATAN_SERVER_DOESNT_EXIST, "server doesn't exist");

	packet p = message_to_packet(msg);

	std::unique_ptr<packet> received_packet = actor_system_.lock()->get_server()->future(p.get_raw_packet(), remote_actor_endpoint_, timeout_ms);
	if (received_packet)
	{
		message<std::string> received_message;
		message::restore_message(received_message, received_packet->data.data);
		return received_message.get_data();
	}
	return "";
}
*/
