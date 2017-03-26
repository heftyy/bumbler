#include "random_pool.h"

#include "../../messages/message.h"

namespace bumbler {

void random_pool::tell_one(std::unique_ptr<message> msg) {
    int actor_index = this->distribution_(this->random_gen_);
    send_message_to_routee(actor_index, std::move(msg));
}

}
