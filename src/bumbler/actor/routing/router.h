#pragma once

#include <memory>
#include "router_pool.h"
#include "../abstract_actor.h"

namespace bumbler {

class actor_system;
class untyped_actor;
class router_pool;

class router : public abstract_actor
{
public:
    router(const std::shared_ptr<actor_system> &actor_system, const std::string &name)
            : abstract_actor(actor_system, name) { }

    void stop_actor(stop_mode stop_mode) override;
    void set_router_pool(std::unique_ptr<router_pool> pool);

protected:
    std::unique_ptr<router_pool> router_pool_;

    virtual void tell(std::unique_ptr<message> msg) override;
};

}
