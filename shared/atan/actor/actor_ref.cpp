#include "actor_ref.h"
#include "../message.h"
#include "../actor_system/actor_system.h"

void actor_ref::tell(const int type, const std::string text, const actor_ref sender) const
{
    message msg(*this, sender, text, type);
    this->tell(msg);
}

void actor_ref::tell(const message& msg) const
{
    int result = actor_system_storage::instance().get_system(system_name)->tell_actor(msg);
    if(result == ACTOR_SYSTEM_STOPPED)
    {
        BOOST_LOG_TRIVIAL(warning) << "[ACTOR_REF] actor system doesn't exist";
    }
}