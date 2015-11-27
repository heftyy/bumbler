#pragma once

#include "../untyped_actor.h"
#include "../routing/router_pool.h"
#include "../mailbox/mailbox.h"

class props {
public:
    bool has_router() const {
        return router_pool_set_;
    }

    bool has_mailbox() const {
        return mailbox_set_;
    }

    bool has_network_actor() const {
        return network_actor_ref_set_;
    }

    virtual std::unique_ptr<actor> create_actor_instance(const std::shared_ptr<actor_system>& actor_system, const std::string name) const = 0;

protected:
    bool router_pool_set_;
    bool mailbox_set_;
    bool network_actor_ref_set_;
};