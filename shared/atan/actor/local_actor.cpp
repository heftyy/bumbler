#include "local_actor.h"
#include "../actor_system/actor_system.h"

actor_ref local_actor::init() {
    this->create_internal_queue_thread();
    return actor::init();
}

void local_actor::tell(std::unique_ptr<message> msg, bool remote) {
    add_message(std::move(msg));
}