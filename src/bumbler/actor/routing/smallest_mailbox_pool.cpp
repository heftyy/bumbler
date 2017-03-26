#include "smallest_mailbox_pool.h"

#include "../../messages/message.h"
#include "../../actor_system/actor_system.h"
#include "../../actor_system/actor_system_storage.h"

namespace bumbler {

void smallest_mailbox_pool::tell_one(std::unique_ptr<message> msg) {
    std::lock_guard<std::mutex> lock(this->mailbox_mutex_);

    int actor_index = 0;
    size_t smallest_mailbox_size = 0;

    auto target_system = actor_system_storage::instance().get_system(msg->get_target().system_key);

    for (int i = 0; i < this->routees_.size(); i++) {
        size_t routee_mailbox_size = target_system->actor_mailbox_size(routees_[i]);
        if(smallest_mailbox_size >= routee_mailbox_size) {
            smallest_mailbox_size = routee_mailbox_size;
            actor_index = i;
        }
    }

    send_message_to_routee(actor_index, std::move(msg));
}

}
