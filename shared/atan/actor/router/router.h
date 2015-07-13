#pragma once

#include "../actor.h"
#include "../../thread_pool/ctpl_stl.h"

class router : public actor
{
protected:
    ctpl::thread_pool thread_pool_;

    router(const std::string& name, std::shared_ptr<actor_system>& actor_system, int size)
            : actor(name, actor_system), size(size) {
        this->thread_pool_.resize(size);
    }

    virtual void tell(std::unique_ptr<message> msg, bool remote = false) = 0;

private:
    int size;
};

