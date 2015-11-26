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
    router(const std::string &name, const std::shared_ptr<actor_system> &actor_system) : actor(name, actor_system) { }

    void stop_actor(bool wait = false) override {
        actor::stop_actor(wait);
        router_pool_->stop(wait);
    }

    void set_router_pool(std::unique_ptr<router_pool> pool) {
        this->router_pool_ = std::move(pool);
    }

protected:
    std::unique_ptr<router_pool> router_pool_;

    template<class T, typename Mailbox, typename ...Args>
    static actor_ref create(std::unique_ptr<router> router_ptr, const std::shared_ptr<actor_system>& actor_system, Args&& ...args) {
        static_assert(
                (std::is_base_of<untyped_actor, T>::value),
                "T has be a derived from untyped_actor"
        );

        auto typed_actor = utility::make_unique<T>(std::forward<Args>(args)...);

        router_ptr->init(std::move(typed_actor));

	    auto& ar = router_ptr->get_self();
        actor::add_to_actor_system(actor_system, std::move(router_ptr));
        return ar;
    }

    virtual void tell(std::unique_ptr<message> msg, bool remote = false) override;
};

