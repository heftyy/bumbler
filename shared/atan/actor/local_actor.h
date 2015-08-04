#pragma once

#include "actor.h"
#include "../actor_system/actor_system_errors.h"

class local_actor : public actor {
protected:
    friend class actor;

    local_actor(const std::string& name, const std::shared_ptr<actor_system>& actor_system)
            : actor(name, actor_system) {
    }

    actor_ref init() override;

    void tell(std::unique_ptr<message> msg, bool remote = false) override;
};
