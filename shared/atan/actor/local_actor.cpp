#include "local_actor.h"
#include "../actor_system/actor_system.h"

void local_actor::tell(std::unique_ptr<message> msg, bool remote) {
    add_message(std::move(msg));
}