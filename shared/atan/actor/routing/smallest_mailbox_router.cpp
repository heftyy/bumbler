#include "smallest_mailbox_router.h"

void smallest_mailbox_router::tell_one(std::unique_ptr<message> msg) {
    std::lock_guard<std::mutex> lock(this->mailbox_mutex_);

    int actor_number = 0;
    unsigned long smallest_mailbox_size = 0;

    for(int i = 0; i < this->actors_.size(); i++) {
        if(smallest_mailbox_size >= actors_[i]->mailbox_size()) {
            smallest_mailbox_size = actors_[i]->mailbox_size();
            actor_number = i;
        }
    }

    this->actors_[actor_number]->add_message(std::move(msg));
}