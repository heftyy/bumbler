#include "remote_actor_channel.h"

#include "../../packet/packet.h"
#include "../../actor/props/typed_props.h"
#include "../../actor/promise_actor.h"
#include "../../actor/typed_promise_actor.h"
#include "../../actor_system/udp_server.h"
#include "../../actor_system/actor_system_storage.h"
#include "../../actor_system/actor_system_errors.h"
#include "../../actor_system/actor_system.h"

namespace bumbler {

bool remote_actor_channel::expired() {
    //TODO: implement remote check alive mechanism
    return false;
}

void remote_actor_channel::tell_impl(std::unique_ptr<message> msg) {
    auto target_system = get_actor_system(msg->get_sender());
    if (!target_system) {
        throw new server_doesnt_exist("can't send messages to remote actors w/o an active actor system");
    }

    auto msg_packet = packet(std::move(msg));

    auto serialized_packet = msg_packet.get_serialized();
    target_system->get_server()->do_send(serialized_packet, network_actor_endpoint_);
}

void remote_actor_channel::ask_impl(std::unique_ptr<message> msg,
                                    const ResponseFun& response_fun) {
    auto target_system = get_actor_system(msg->get_sender());
    if (!target_system) {
        throw new server_doesnt_exist("can't send messages to remote actors w/o an active actor system");
    }

    auto response_promise_actor = typed_props<promise_actor, typed_promise_actor>(response_fun);
    actor_ref p_actor = target_system->actor_of(response_promise_actor,
                                                target_system->get_next_temporary_actor_name());

    msg->set_sender(p_actor);

    auto msg_packet = packet(std::move(msg));

    auto serialized_packet = msg_packet.get_serialized();
    target_system->get_server()->do_send(serialized_packet, network_actor_endpoint_);
}

std::shared_ptr<actor_system> remote_actor_channel::get_actor_system(const actor_ref& sender) const {
    if (sender.is_none() || sender.is_remote()) {
        return actor_system_storage::instance().any();
    }
    else {
        return actor_system_storage::instance().get_system(sender.system_name);
    }
}

boost::asio::ip::udp::endpoint remote_actor_channel::get_remote_endpoint(
        const actor_ref& network_actor_ref) const {
    return boost::asio::ip::udp::endpoint(
            boost::asio::ip::address().from_string(network_actor_ref.ip),
            static_cast<unsigned short>(network_actor_ref.port)
    );
}

}