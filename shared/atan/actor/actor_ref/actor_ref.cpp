#include "actor_ref.h"
#include "../promise_actor.h"
#include "../../actor_system/actor_system.h"

void actor_ref::stop() {
    this->tell(stop_actor<int>(0));
}

void actor_ref::kill() {
    this->tell(kill_actor<int>(0));
}

void actor_ref::actor_tell(std::unique_ptr<message> msg) const {
    int result = actor_system_storage::instance().get_system(system_name)->tell_actor(std::move(msg));
    if (result == ACTOR_SYSTEM_STOPPED) {
        BOOST_LOG_TRIVIAL(warning) << "[ACTOR_REF] actor system doesn't exist";
    }
}

void actor_ref::actor_ask_system(std::unique_ptr<message> msg, std::function<void(boost::any)>& response_fn) const {
    int result = actor_system_storage::instance().get_system(system_name)->ask_actor(std::move(msg), response_fn);
    if (result == ACTOR_SYSTEM_STOPPED) {
        BOOST_LOG_TRIVIAL(warning) << "[ACTOR_REF] actor system doesn't exist";
    }
}
