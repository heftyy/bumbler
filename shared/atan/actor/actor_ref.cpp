#include "actor_ref.h"
#include "../message.h"
#include "../actor_system/actor_system.h"

void actor_ref::tell_(std::unique_ptr<message> msg) {
    int result = actor_system_storage::instance().get_system(system_name)->tell_actor(std::move(msg));
    if (result == ACTOR_SYSTEM_STOPPED) {
        BOOST_LOG_TRIVIAL(warning) << "[ACTOR_REF] actor system doesn't exist";
    }
}