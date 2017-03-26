#include "router_pool.h"
#include "../actor_ref/actor_ref.h"
#include "../../messages/message.h"
#include "../../internal/messaging_service.h"
#include "../../actor/mailbox/mailbox.h"
#include "../../actor/untyped_actor.h"
#include "../../actor/abstract_actor.h"
#include "../../actor_system/actor_system.h"

namespace bumbler {

void router_pool::tell(std::unique_ptr<message> msg) {
    assert(routees_.size() > 0 && "router pool has to be initialized before you use tell");

    if (msg->is_broadcast()) {
        this->tell_all(std::move(msg));
    }
    else {
        this->tell_one(std::move(msg));
    }
}

void router_pool::create(const std::shared_ptr<actor_system>& actor_system, const std::string& router_name, 
                         const ActorCreateFun& get_actor_func, const MailboxCreateFun& get_mailbox_func, const TypedActorCreateFun& get_typed_actor_func) {
    std::string next_router_name = router_name + "/a";

    for (int i = 0; i < this->pool_size_; i++) {
        auto actor = get_actor_func(actor_system, next_router_name);
        actor->set_mailbox(get_mailbox_func());
        actor->init(get_typed_actor_func());
        actor_system->add_actor(std::move(actor));
        routees_.push_back(next_router_name);

        if (next_router_name[next_router_name.length() - 1] == 'z') {
            next_router_name += 'a';
        }
        else {
            next_router_name[next_router_name.length() - 1]++;
        }
    }
}

void router_pool::tell_all(std::unique_ptr<message> msg) {
    for (int i = 0; i < this->routees_.size(); i++) {
        auto msg_copy = msg->clone();

        send_message_to_routee(i, std::move(msg_copy));
    }
}

void router_pool::send_message_to_routee(int routee_index, std::unique_ptr<message> msg) {
    actor_ref target_ref = msg->get_target();
    target_ref.actor_key = routees_[routee_index];
    msg->set_target(target_ref);

    messaging_service::send_message(std::move(msg));
}

}
