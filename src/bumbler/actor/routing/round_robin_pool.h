#pragma once

#include <vector>
#include <memory>
#include "router_pool.h"

namespace bumbler {

class round_robin_pool : public router_pool {
public:
    round_robin_pool(int pool_size)
            : router_pool(pool_size, router_logic::ROUND_ROBIN_LOGIC), next_actor_to_message_(0) { }

    round_robin_pool(round_robin_pool&& rhs) = default; // support moving
    round_robin_pool& operator=(round_robin_pool&& rhs) = default;

    virtual ~round_robin_pool() {}

    void tell_one(std::unique_ptr<message> msg) override;

    virtual std::unique_ptr<router_pool> clone() const override  {
        return std::unique_ptr<router_pool>(new round_robin_pool(*this));
    }

protected:
    round_robin_pool(const round_robin_pool& rhs) :
            router_pool(rhs),
            next_actor_to_message_(rhs.next_actor_to_message_.load()) { }

    round_robin_pool& operator=(const round_robin_pool& rhs) {
        if(this != &rhs) {
            router_pool::operator=(rhs);
            next_actor_to_message_.store(rhs.next_actor_to_message_);
        }
        return *this;
    }

private:
    std::atomic<int> next_actor_to_message_;
};

}
