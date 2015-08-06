#include "router.h"
#include "../../actor_system/actor_system.h"

actor_ref router::init() {
    return actor::init();
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
    for(int i = 0; i < this->actors.size(); i++) {
        std::unique_ptr<message> msg_copy = std::move(msg->clone());

        actors[i]->add_message(std::move(msg_copy));
    }
}