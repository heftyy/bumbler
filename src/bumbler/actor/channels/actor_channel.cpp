#include "actor_channel.h"
#include "../actor_ref/actor_ref.h"

namespace bumbler {

actor_channel::actor_channel(const actor_ref& target_actor_ref) :
        actor_ref_(std::make_unique<actor_ref>(target_actor_ref)) { }

}
