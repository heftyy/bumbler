#pragma once

#include <vector>
#include <memory>
#include "router_pool.h"

class round_robin_pool : public router_pool {
public:
    round_robin_pool(int pool_size)
            : router_pool(pool_size, router_logic::ROUND_ROBIN_LOGIC), current_actor_to_message_(0) { }

    round_robin_pool(round_robin_pool&& rhs) = default; // support moving
    round_robin_pool& operator=(round_robin_pool&& rhs) = default;
    round_robin_pool(const round_robin_pool& rhs) :
            router_pool(rhs),
            current_actor_to_message_(rhs.current_actor_to_message_.load()) { }
    round_robin_pool& operator=(const round_robin_pool& rhs) {
        if(this != &rhs) {
            router_pool::operator=(rhs);
            current_actor_to_message_.store(rhs.current_actor_to_message_);
        }
        return *this;
    }
    virtual ~round_robin_pool() {}

    void tell_one(std::unique_ptr<message> msg) override;

    virtual std::unique_ptr<router_pool> clone() const override  {
        return std::unique_ptr<router_pool>(new round_robin_pool(*this));
    }

private:
    std::atomic<int> current_actor_to_message_;
};

