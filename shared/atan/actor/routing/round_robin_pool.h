#pragma once

#include <vector>
#include <memory>
#include "router_pool.h"

class round_robin_pool : public router_pool {
public:
    round_robin_pool(int pool_size)
            : router_pool(pool_size, router_logic::ROUND_ROBIN_LOGIC) { }

    void tell_one(std::unique_ptr<message> msg) override;

private:
    std::atomic<int> current_actor_to_message_;
};

