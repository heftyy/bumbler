#include "messaging_service.h"

#include "../internal/identifier.h"
#include "../messages/message.h"
#include "../actor_system/actor_system.h"
#include "../actor_system/actor_system_storage.h"

namespace bumbler {

void messaging_service::send_message(std::unique_ptr<message> msg) {
    const actor_ref& target_ref = msg->get_target();
    auto target_system = actor_system_storage::instance().get_system(target_ref.system_key);
    if (target_system) {
        target_system->tell_actor(std::move(msg));
    }
}

}
