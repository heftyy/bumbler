#include "smallest_mailbox_pool.h"

namespace bumbler {

void smallest_mailbox_pool::tell_one(std::unique_ptr<message> msg) {
    std::lock_guard<std::mutex> lock(this->mailbox_mutex_);

    int actor_number = 0;
    size_t smallest_mailbox_size = 0;

    for(int i = 0; i < this->routees_.size(); i++) {
        if(smallest_mailbox_size >= routees_[i]->mailbox_size()) {
            smallest_mailbox_size = routees_[i]->mailbox_size();
            actor_number = i;
        }
    }

    this->routees_[actor_number]->add_message(std::move(msg));
}

}
