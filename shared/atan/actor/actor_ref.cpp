#include "actor_ref.h"
#include "../message.h"
#include "../actor_system/actor_system.h"

template<typename T>
void actor_ref::tell_(T data, actor_ref sender)
{
    message<T> msg = message<T>(*this, sender, data, 1);
    int result = actor_system_storage::instance().get_system(system_name)->tell_actor(msg);
    if(result == ACTOR_SYSTEM_STOPPED)
    {
        BOOST_LOG_TRIVIAL(warning) << "[ACTOR_REF] actor system doesn't exist";
    }
}