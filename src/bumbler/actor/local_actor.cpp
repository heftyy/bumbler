#include "local_actor.h"
#include "../actor_system/actor_system.h"

namespace bumbler {

void local_actor::init(std::unique_ptr<untyped_actor> u_actor) {
    abstract_actor::init(std::move(u_actor));
    this->create_internal_queue_thread();
}

void local_actor::tell(std::unique_ptr<message> msg) {
    add_message(std::move(msg));
}

}
