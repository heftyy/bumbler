#include "actor_ref.h"
#include "../promise_actor.h"
#include "../../actor_system/actor_system.h"
#include "../channels/local_actor_channel.h"
#include "../channels/remote_actor_channel.h"

namespace bumbler {

std::string actor_ref::to_string() const {
    std::stringstream ss;
    ss << actor_name << "$" << system_name << "@" << ip << ":" << port;
    return ss.str();
}

void actor_ref::stop() {
    this->tell(stop_actor<int>(0));
}

void actor_ref::kill() {
    this->tell(kill_actor<int>(0));
}

void actor_ref::resolve() {
    if(is_remote()) {
        channel_ = utility::make_unique<remote_actor_channel>(*this);
    } else {
        channel_ = utility::make_unique<local_actor_channel>(*this);
    }
}

}
