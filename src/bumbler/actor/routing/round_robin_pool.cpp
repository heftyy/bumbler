
#include "round_robin_pool.h"

#include "../../messages/message.h"

namespace bumbler {

void round_robin_pool::tell_one(std::unique_ptr<message> msg) {
    int actor_index = this->next_actor_to_message_++;
    send_message_to_routee(actor_index, std::move(msg));

    if(this->next_actor_to_message_ >= this->pool_size_) {
        this->next_actor_to_message_ = 0;
    }
}

}
