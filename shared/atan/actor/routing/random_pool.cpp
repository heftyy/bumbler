#include "random_pool.h"

void random_pool::tell_one(std::unique_ptr<message> msg) {
    int actor_number = this->distribution_(this->random_gen_);
    this->routees_[actor_number]->add_message(std::move(msg));
}
