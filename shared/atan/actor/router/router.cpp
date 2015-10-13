#include "router.h"
#include "../../actor_system/actor_system.h"

void router::init(std::unique_ptr<untyped_actor> u_actor) {
    actor::init(std::move(u_actor));
}

void router::tell(std::unique_ptr<message> msg, bool remote) {
    if(msg->is_broadcast()) {
        this->tell_all(std::move(msg));
    }
    else {
        this->tell_one(std::move(msg));
    }
}

void router::tell_all(std::unique_ptr<message> msg) {
    for(int i = 0; i < this->actors_.size(); i++) {
	    auto msg_copy = std::move(msg->clone_ptr());

        actors_[i]->add_message(std::move(msg_copy));
    }
}