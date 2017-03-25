#pragma once

#include <memory>
#include <atomic>
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

protected:
    round_robin_pool(const round_robin_pool& rhs) = delete;
    round_robin_pool& operator=(const round_robin_pool& rhs) = delete;

private:
    std::atomic<int> next_actor_to_message_;
};

}
