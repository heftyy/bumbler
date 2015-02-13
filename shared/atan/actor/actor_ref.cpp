#include "actor_ref.h"
#include "../message.h"
#include "../actor_system/actor_system.h"

void actor_ref::tell(const int type, const std::string text, const actor_ref sender)
{
    message msg(*this, sender, text, type);
    this->tell(msg);
}

void actor_ref::tell(const message& msg)
{
    actor_system_storage::instance().get_system(system_name)->tell_actor(msg);
}