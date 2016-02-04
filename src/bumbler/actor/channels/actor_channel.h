#pragma once

#include "abstract_channel.h"

namespace bumbler {

class actor_ref;

class actor_channel : public abstract_channel {
protected:
    actor_channel() : abstract_channel() { }

    actor_channel(const actor_ref& target_actor_ref);

    std::unique_ptr<actor_ref> actor_ref_;
};

}
