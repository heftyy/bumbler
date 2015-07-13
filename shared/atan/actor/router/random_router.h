#pragma once

#include "router.h"

class random_router : public router
{
public:
    friend class actor;
protected:

    random_router(const std::string& name, std::shared_ptr<actor_system>& actor_system, int size)
            : router(name, actor_system, size) {
    }

    actor_ref init();

    void tell(std::unique_ptr<message> msg, bool remote = false);
};

