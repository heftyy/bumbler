#include "remote_actor.h"
#include "../actor_system/actor_system.h"

void remote_actor::init(std::unique_ptr<untyped_actor> u_actor) {
    actor::init(std::move(u_actor));
    this->create_internal_queue_thread();
}

void remote_actor::tell(std::unique_ptr<message> msg, bool remote) {
    //if remote is true the message if from another application so we should just send it to on_receive
    if(remote) {
        add_message(std::move(msg));
        return;
    }

    //replace the target with the remote one
    msg->set_target(this->network_actor_ref_);
    if(msg->get_sender().is_none()) {
        msg->set_sender(get_self());
    }

    packet p = message_to_packet(std::move(msg));
    tell_(p);
}

void remote_actor::tell_(packet& p) {
    if (!actor_system_.lock()->get_server()) atan_error(ATAN_SERVER_DOESNT_EXIST, "server doesn't exist");

    std::string serialized_packet = p.get_serialized();

    actor_system_.lock()->get_server()->do_send(serialized_packet, remote_actor_endpoint_);
}