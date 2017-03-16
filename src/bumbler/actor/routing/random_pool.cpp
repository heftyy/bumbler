#include "random_pool.h"

#include "../abstract_actor.h"
#include "../../messages/message.h"

namespace bumbler {

void random_pool::tell_one(std::unique_ptr<message> msg) {
    int actor_number = this->distribution_(this->random_gen_);
    this->routees_[actor_number]->pass_message(std::move(msg));
}

}
