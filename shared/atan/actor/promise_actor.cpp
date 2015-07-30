#include "promise_actor.h"
#include "../actor_system/actor_system.h"

void promise_actor::on_receive(boost::any data) {
    response_fn_(data);
    //don't wait for this actor to finish tasks because it shouldn't have any more
    this->actor_system_.lock()->stop_actor(this->actor_name(), false);
}