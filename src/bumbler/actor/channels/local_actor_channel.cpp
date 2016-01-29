#include "../../actor_system/actor_system_storage.h"
#include "../../actor_system/actor_system.h"
#include "local_actor_channel.h"

void bumbler::local_actor_channel::tell_impl(std::unique_ptr<bumbler::message> msg) const {
    auto target_system = actor_system_storage::instance().get_system(msg->get_target().actor_name);
    if(target_system != nullptr) {
        target_system->tell_actor(std::move(msg));
    }
}

void bumbler::local_actor_channel::ask_impl(std::unique_ptr<bumbler::message> msg,
                                            const std::function<void(boost::any)>& response_fn) const {


}
