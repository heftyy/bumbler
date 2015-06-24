#pragma once

#include "actor.h"
#include "../actor_system/actor_system_errors.h"

class local_actor : public actor {
protected:
    local_actor(const std::string& name, std::shared_ptr<actor_system>& actor_system)
            : actor(name, actor_system) {
    }

    void tell(std::unique_ptr<message> msg, bool remote = false);
};
