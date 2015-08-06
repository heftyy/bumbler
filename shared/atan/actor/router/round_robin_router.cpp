#include "round_robin_router.h"

void round_robin_router::tell_one(std::unique_ptr<message> msg) {

    int actor_number = this->current_actor_to_message++;
    this->actors[actor_number]->add_message(std::move(msg));

    if(this->current_actor_to_message >= this->size) {
        this->current_actor_to_message = 0;
    }
}