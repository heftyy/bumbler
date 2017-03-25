#include "router_pool.h"
#include "../abstract_actor.h"
#include "../../messages/message.h"

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

void router_pool::stop(stop_mode stop_mode) {
    for (auto&& routee : this->routees_) {
        routee->stop_actor(stop_mode);
    }
}

void router_pool::tell_all(std::unique_ptr<message> msg) {
    for (int i = 0; i < this->routees_.size(); i++) {
        auto msg_copy = std::move(msg->clone());

        routees_[i]->pass_message(std::move(msg_copy));
    }
}

}
