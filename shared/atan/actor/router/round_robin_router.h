#pragma once

#include <atomic>
#include "router.h"

class round_robin_router : public router
{
public:
    friend class actor;

    round_robin_router(const std::string& name, std::shared_ptr<actor_system>& actor_system, int size)
            : router(name, actor_system, size) {
        this->current_actor_to_message = 0;
    }

    void tell(std::unique_ptr<message> msg, bool remote = false);

private:
    std::atomic<int> current_actor_to_message;
};

