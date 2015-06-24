#include "remote_actor.h"
#include "../actor_system/actor_system.h"

void remote_actor::tell(std::unique_ptr<message> msg, bool remote) {
    //if remote is true the message if from another application so we should just send it to on receive
    if(remote) {
        add_message(std::move(msg));
        return;
    }

    //replace the target with the remote one
    msg->set_target(this->network_actor_ref_);
    if(!msg->get_sender().exists()) {
        msg->set_sender(get_self());
    }

    packet p = message_to_packet(std::move(msg));
    tell_(p);
}

void remote_actor::tell_(packet& p) {
    if (!actor_system_.lock()->get_server()) atan_error(ATAN_SERVER_DOESNT_EXIST, "server doesn't exist");

    std::string serialized_packet = p.get_serialized();

    BOOST_LOG_TRIVIAL(debug) << "Serialized packet: \n" << serialized_packet;

    actor_system_.lock()->get_server()->do_send(serialized_packet, remote_actor_endpoint_);
}

/*
template<typename T>
boost::any remote_actor::future_(message<T>& msg, int timeout_ms)
{
	if (!actor_system_.lock()->get_server()) atan_error(ATAN_SERVER_DOESNT_EXIST, "server doesn't exist");

	packet p = message_to_packet(msg);

	std::unique_ptr<packet> received_packet = actor_system_.lock()->get_server()->future(p.get_serialized(), remote_actor_endpoint_, timeout_ms);
	if (received_packet)
	{
		message<std::string> received_message;
		message::restore_message(received_message, received_packet->data.data);
		return received_message.get_data();
	}
	return "";
}
*/