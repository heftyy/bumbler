#include "local_actor_channel.h"
#include "../actor_ref/actor_ref.h"
#include "../../actor/promise_actor.h"
#include "../../actor/typed_promise_actor.h"
#include "../../actor/props/typed_props.h"
#include "../../actor_system/actor_system_storage.h"
#include "../../actor_system/actor_system.h"

namespace bumbler {

bool local_actor_channel::expired() {
    if (!system_.expired()) {
        return !system_.lock()->has_actor(actor_identifier_);
    }
    return true;
}

void local_actor_channel::tell_impl(std::unique_ptr<message> msg) {
    const actor_ref& target = msg->get_target();
    actor_system_storage::instance().get_system(target.system_key)->tell_actor(std::move(msg));
}

void local_actor_channel::ask_impl(std::unique_ptr<message> msg, const ResponseFun& response_fun) {
    const actor_ref& target = msg->get_target();
    auto target_system = actor_system_storage::instance().get_system(target.system_key);
    if (target_system != nullptr) {
        auto response_promise_actor = typed_props<promise_actor, typed_promise_actor>(response_fun);
        actor_ref p_actor = target_system->actor_of(response_promise_actor,
                                                    target_system->get_next_temporary_actor_name());

        msg->set_sender(p_actor);
        target_system->tell_actor(std::move(msg));
    }
}

}
