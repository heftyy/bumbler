#pragma once

#include "abstract_channel.h"

namespace bumbler {

class actor_channel : public abstract_channel {
protected:
    actor_channel() : abstract_channel() { }

    actor_channel(const actor_ref& actor_ref_, const std::shared_ptr<abstract_actor>& actor_ptr_) :
            abstract_channel(actor_ref_, actor_ptr_) { }
};

}