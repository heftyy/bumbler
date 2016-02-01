#include "local_actor_channel.h"
#include "../../actor_system/actor_system_storage.h"
#include "../../actor_system/actor_system.h"

namespace bumbler {

void local_actor_channel::tell_impl(std::unique_ptr<message> msg) {
    auto target_system = actor_system_storage::instance().get_system(msg->get_target().system_name);
    if (target_system != nullptr) {
        target_system->tell_actor(std::move(msg));
    }
}

void local_actor_channel::ask_impl(std::unique_ptr<message> msg,
                                            const std::function<void(boost::any)>& response_fn) {
    auto target_system = actor_system_storage::instance().get_system(msg->get_target().system_name);
    if (target_system != nullptr) {
        auto response_promise_actor = typed_props<promise_actor, typed_promise_actor>(response_fn);
        actor_ref p_actor = target_system->actor_of(response_promise_actor,
                                                    target_system->get_next_temporary_actor_name());

        msg->set_sender(p_actor);

        target_system->tell_actor(std::move(msg));
    }

}

}