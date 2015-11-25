#include "round_robin_pool.h"

void round_robin_pool::tell_one(std::unique_ptr<message> msg) {
    int actor_number = this->current_actor_to_message_++;
    this->routees_[actor_number]->add_message(std::move(msg));

    if(this->current_actor_to_message_ >= this->pool_size_) {
        this->current_actor_to_message_ = 0;
    }
}
