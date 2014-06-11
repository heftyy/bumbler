#include "remote_actor.h"
#include "../actor_system/actor_system.h"

void remote_actor::tell_(message& msg)
{
	if (!actor_system_.server()) atan_error(ATAN_SERVER_DOESNT_EXIST, "server doesn't exist");

	packet p = message_to_packet(msg);

	actor_system_.server()->do_send(p.get_raw_packet(), remote_actor_endpoint_);
}

message remote_actor::future_(message& msg, int timeout_ms)
{
	if (!actor_system_.server()) atan_error(ATAN_SERVER_DOESNT_EXIST, "server doesn't exist");

	packet p = message_to_packet(msg);

	std::unique_ptr<packet> received_packet = actor_system_.server()->future(p.get_raw_packet(), remote_actor_endpoint_, timeout_ms);
	if (received_packet)
	{
		message received_message;
		message::restore_message(received_message, received_packet->data.data);
		return received_message;
	}
	return message();
}
