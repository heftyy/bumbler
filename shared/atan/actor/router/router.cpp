#include "router.h"
#include "../../actor_system/actor_system.h"

actor_ref router::init() {
    this->thread_pool.resize(size);

    this->actor_system_.lock()->add_actor(shared_from_this());
    return this->get_self();
}