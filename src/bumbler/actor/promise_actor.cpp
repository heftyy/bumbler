#include "promise_actor.h"
#include "../actor_system/actor_system.h"

void promise_actor::tell(std::unique_ptr<message> msg, bool remote) {
    on_receive(msg->get_data());

    //don't wait for this actor to finish tasks because it shouldn't have any more
    this->actor_system_.lock()->stop_actor(this->actor_name(), false);
}
