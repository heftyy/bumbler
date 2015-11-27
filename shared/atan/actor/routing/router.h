#pragma once

#include <list>
#include <memory>
#include <functional>
#include "../actor.h"
#include "../local_actor.h"
#include "router_pool.h"

class actor_system;
class untyped_actor;

class router : public actor
{
public:
    router(const std::shared_ptr<actor_system> &actor_system, const std::string &name)
            : actor(actor_system, name) { }

    void stop_actor(bool wait = false);
    void set_router_pool(std::unique_ptr<router_pool> pool);

protected:
    std::unique_ptr<router_pool> router_pool_;

    virtual void tell(std::unique_ptr<message> msg, bool remote = false) override;
};

