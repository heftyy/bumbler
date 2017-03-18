#include "actor_ref.h"
#include "../promise_actor.h"
#include "../channels/local_actor_channel.h"
#include "../channels/remote_actor_channel.h"
#include "../../messages/commands/commands.h"
#include "../../messages/typed_message.h"
#include "../../actor_system/actor_system.h"
#include "../../actor_system/actor_system_storage.h"

namespace bumbler {

std::string actor_ref::to_string() const {
    std::stringstream ss;
    ss << actor_name << "$" << system_name << "@" << ip << ":" << port;
    return ss.str();
}

void actor_ref::stop() {
    this->tell(stop_actor(0));
}

void actor_ref::kill() {
    this->tell(kill_actor(0));
}

void actor_ref::resolve() {
    if(is_remote()) {
        channel_ = std::make_unique<remote_actor_channel>(*this);
    } else {
        channel_ = actor_system_storage::instance().get_system(system_name)->get_actor_channel(actor_name);
    }
}

std::unique_ptr<message> actor_ref::make_message(std::unique_ptr<variant> variant_ptr, const actor_ref& target, const actor_ref& sender) const {
	return typed_message_factory::create(target, sender, std::move(variant_ptr));
}

}
