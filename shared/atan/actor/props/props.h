#pragma once

#include "../untyped_actor.h"
#include "../routing/router_pool.h"
#include "../mailbox/mailbox.h"

class props {
public:
    props& with_router(std::unique_ptr<router_pool> pool) {
        router_pool_ = std::move(pool);
        return *this;
    }

    props& with_mailbox(std::unique_ptr<mailbox> mailbox) {
        mailbox_ = std::move(mailbox);
        return *this;
    }

    virtual std::unique_ptr<actor> create_actor_instance(std::string name, const std::shared_ptr<actor_system>& actor_system) = 0;

protected:
    std::unique_ptr<untyped_actor> untyped_actor_;
    std::unique_ptr<router_pool> router_pool_;
    std::unique_ptr<mailbox> mailbox_;
};