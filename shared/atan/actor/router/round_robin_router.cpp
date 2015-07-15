#include "round_robin_router.h"

void round_robin_router::tell_actor(std::unique_ptr<message> msg) {
    this->thread_pool.push(
            std::bind(
                    [this] (std::unique_ptr<message>& m, int actor_id) {
                        BOOST_LOG_TRIVIAL(debug) << "CURRENT ACTOR IS = " << actor_id;
                        this->actors[actor_id]->on_receive(m->get_data());
                    }, std::move(msg), this->current_actor_to_message++
            )
    );

    if(this->current_actor_to_message >= this->size) {
        this->current_actor_to_message = 0;
    }
}