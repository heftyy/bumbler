#include "actor_ref.h"
#include "../actor_system/actor_system.h"

void actor_ref::tell_(std::unique_ptr<message> msg) const {
    int result = actor_system_storage::instance().get_system(system_name)->tell_actor(std::move(msg));
    if (result == ACTOR_SYSTEM_STOPPED) {
        BOOST_LOG_TRIVIAL(warning) << "[ACTOR_REF] actor system doesn't exist";
    }
}

/*
bool actor_ref::exists() const {
    std::shared_ptr<actor_system> system = actor_system_storage::instance().get_system(system_name);
    if(system != nullptr) {
        actor_ref ref = system->get_actor(actor_name);
        return !ref.is_none();
    }
    return false;
}
*/

void actor_ref::stop() {
    this->tell(stop_actor<int>(0));
}

void actor_ref::kill() {
    this->tell(kill_actor<int>(0));
}