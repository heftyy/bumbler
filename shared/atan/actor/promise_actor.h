#pragma once

#include <future>
#include "actor.h"

class promise_actor : public actor {
public:
    promise_actor(const std::shared_ptr<actor_system>& actor_system, const std::string name)
            : actor(actor_system, name) {
    }

protected:
    void tell(std::unique_ptr<message> msg, bool remote = false) override;
};
