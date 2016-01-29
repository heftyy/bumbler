#pragma once

#include <future>
#include "abstract_actor.h"

class promise_actor : public abstract_actor {
public:
    promise_actor(const std::shared_ptr<actor_system>& actor_system, const std::string name)
            : abstract_actor(actor_system, name) {
    }

protected:
    void tell(std::unique_ptr<message> msg, bool remote = false) override;
};
