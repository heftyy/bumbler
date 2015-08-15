#pragma once

#include <random>
#include <boost/log/trivial.hpp>
#include "router.h"

class random_router : public router
{
public:
    friend class actor;

    template<class T, typename ...Args>
    static actor_ref create(const std::string& name, const std::shared_ptr<actor_system>& actor_system, int size, Args&& ...args) {
	    auto router_ptr = std::unique_ptr<random_router>(new random_router(name, actor_system, size));
        return router::create<T>(std::move(router_ptr), actor_system, std::forward<Args>(args)...);
    }
protected:

    random_router(const std::string& name, const std::shared_ptr<actor_system>& actor_system, int size)
            : router(name, actor_system, size) {
        this->random_gen_ = std::mt19937(rd_());
        this->distribution_ = std::uniform_int_distribution<>(0, this->size-1);
    }

    void tell_one(std::unique_ptr<message> msg) override;

private:
    std::random_device rd_;
    std::mt19937 random_gen_;
    std::uniform_int_distribution<> distribution_;
};

