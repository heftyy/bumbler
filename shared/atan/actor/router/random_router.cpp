#include "random_router.h"

void random_router::tell_one(std::unique_ptr<message> msg) {
    int actor_number = this->distribution_(this->random_gen_);
    this->actors[actor_number]->add_message(std::move(msg));
}