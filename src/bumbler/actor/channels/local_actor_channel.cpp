#include "local_actor_channel.h"
#include "../../actor/abstract_actor.h"
#include "../../actor/promise_actor.h"
#include "../../actor/typed_promise_actor.h"
#include "../../actor/props/typed_props.h"
#include "../../actor_system/actor_system_storage.h"
#include "../../actor_system/actor_system.h"

namespace bumbler {

bool local_actor_channel::expired() {
    return actor_ptr_.expired();
}

void local_actor_channel::tell_impl(std::unique_ptr<message> msg) {
    actor_ptr_.lock()->pass_message(std::move(msg));
}

void local_actor_channel::ask_impl(std::unique_ptr<message> msg,
                                   const ResponseFun& response_fun) {
    auto target_system = actor_system_storage::instance().get_system(msg->get_target().system_name);
    if (target_system != nullptr) {
        auto response_promise_actor = typed_props<promise_actor, typed_promise_actor>(response_fun);
        actor_ref p_actor = target_system->actor_of(response_promise_actor,
                                                    target_system->get_next_temporary_actor_name());

        msg->set_sender(p_actor);

        actor_ptr_.lock()->tell(std::move(msg));
    }

}

}