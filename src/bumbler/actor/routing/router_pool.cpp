#include "router_pool.h"
#include "../actor_ref/actor_ref.h"
#include "../../messages/message.h"
#include "../../internal/messaging_service.h"

namespace bumbler {

void router_pool::tell(std::unique_ptr<message> msg) {
    assert(routees_.size() > 0 && "router pool has to be initialized before you use tell");

    if (msg->is_broadcast()) {
        this->tell_all(std::move(msg));
    }
    else {
        this->tell_one(std::move(msg));
    }
}

void router_pool::tell_all(std::unique_ptr<message> msg) {
    for (int i = 0; i < this->routees_.size(); i++) {
        auto msg_copy = msg->clone();

        send_message_to_routee(i, std::move(msg_copy));
    }
}

void router_pool::send_message_to_routee(int routee_index, std::unique_ptr<message> msg) {
    actor_ref target_ref = msg->get_target();
    target_ref.actor_key = routees_[routee_index];
    msg->set_target(target_ref);

    messaging_service::send_message(std::move(msg));
}

}
