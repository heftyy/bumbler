#include "promise_actor.h"
#include "../messages/message.h"
#include "../actor_system/actor_system.h"

namespace bumbler {

void promise_actor::tell(std::unique_ptr<message> msg) {
    on_receive(msg->get_data());

    //don't wait for this actor to finish tasks because it shouldn't have any more
    this->actor_system_.lock()->stop_actor(actor_key_, stop_mode::IGNORE_QUEUE);
}

}
