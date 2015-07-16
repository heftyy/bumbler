#include "router.h"
#include "../../actor_system/actor_system.h"

actor_ref router::init() {
    this->thread_pool.resize(size);

    this->actor_system_.lock()->add_actor(shared_from_this());
    return this->get_self();
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
    std::shared_ptr<message> shared_message(std::move(msg));

    for(int i = 0; i < this->actors.size(); i++) {
        this->thread_pool.push(
                std::bind(
                        [this] (std::shared_ptr<message>& m, int actor_id) {
                            BOOST_LOG_TRIVIAL(debug) << "CURRENT ACTOR IS = " << actor_id;
                            this->actors[actor_id]->on_receive(m->get_data());
                        }, shared_message, i
                )
        );
    }
}