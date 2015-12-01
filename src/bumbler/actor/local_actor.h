#pragma once

#include "actor.h"
#include "../actor_system/actor_system_errors.h"

class actor_system;
class untyped_actor;

class local_actor : public actor {
public:
    local_actor(const std::shared_ptr<actor_system>& actor_system, const std::string name)
            : actor(actor_system, name) {
    }

    void init(std::unique_ptr<untyped_actor> u_actor) override;

protected:
    void tell(std::unique_ptr<message> msg, bool remote = false) override;
};
