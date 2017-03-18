#pragma once

#include "abstract_actor.h"

namespace bumbler {

class promise_actor : public abstract_actor {
public:
    promise_actor(const std::shared_ptr<actor_system>& actor_system, const std::string& name)
            : abstract_actor(actor_system, name) {
    }

    void stop_actor(stop_mode stop_mode) override { };
    void tell(std::unique_ptr<message> msg) override;
};

}
