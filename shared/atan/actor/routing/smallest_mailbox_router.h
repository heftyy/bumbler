#pragma once

#include <atomic>
#include <mutex>
#include "router.h"

class smallest_mailbox_router : public router
{
public:
    template<class T, class Mailbox = fifo_mailbox, typename ...Args>
    static actor_ref create(std::string name, const std::shared_ptr<actor_system>& actor_system, int size, Args&& ...args) {
	    auto router_ptr = std::unique_ptr<smallest_mailbox_router>(new smallest_mailbox_router(name, actor_system, size));
        return router::create<T, Mailbox>(std::move(router_ptr), actor_system, std::forward<Args>(args)...);
    }

protected:
    friend class actor;

    smallest_mailbox_router(const std::string name, const std::shared_ptr<actor_system>& actor_system, int size)
            : router(name, actor_system, size) { }

private:
    std::mutex mailbox_mutex_;

};

