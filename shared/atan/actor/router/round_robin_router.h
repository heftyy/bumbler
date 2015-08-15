#pragma once

#include <atomic>
#include "router.h"

class round_robin_router : public router
{
public:
    template<class T, typename ...Args>
    static actor_ref create(const std::string& name, const std::shared_ptr<actor_system>& actor_system, int size, Args&& ...args) {
	    auto router_ptr = std::unique_ptr<round_robin_router>(new round_robin_router(name, actor_system, size));
        return router::create<T>(std::move(router_ptr), actor_system, std::forward<Args>(args)...);
    }

protected:
    friend class actor;

    round_robin_router(const std::string& name, const std::shared_ptr<actor_system>& actor_system, int size)
            : router(name, actor_system, size) {
        this->current_actor_to_message = 0;
    }

    void tell_one(std::unique_ptr<message> msg) override;

private:
    std::atomic<int> current_actor_to_message;
};

