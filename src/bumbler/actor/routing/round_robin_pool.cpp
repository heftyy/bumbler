#include "round_robin_pool.h"

namespace bumbler {

void round_robin_pool::tell_one(std::unique_ptr<message> msg) {
    int actor_number = this->next_actor_to_message_++;
    this->routees_[actor_number]->pass_message(std::move(msg));

    if(this->next_actor_to_message_ >= this->pool_size_) {
        this->next_actor_to_message_ = 0;
    }
}

}
