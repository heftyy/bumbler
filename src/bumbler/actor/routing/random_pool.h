#pragma once

#include <memory>
#include <random>
#include "router_pool.h"

namespace bumbler {

class random_pool : public router_pool {
public:
    random_pool(int pool_size)
            : router_pool(pool_size, router_logic::RANDOM_LOGIC) {
        this->random_gen_ = std::mt19937(rd_());
        this->distribution_ = std::uniform_int_distribution<>(0, this->pool_size_ -1);
    }

    random_pool(random_pool&& rhs) = default; // support moving
    random_pool& operator=(random_pool&& rhs) = default;

    virtual ~random_pool() {}

    void tell_one(std::unique_ptr<message> msg) override;

protected:
    random_pool(const random_pool& rhs) = delete;
    random_pool& operator=(const random_pool& rhs) = delete;

private:
    std::random_device rd_;
    std::mt19937 random_gen_;
    std::uniform_int_distribution<> distribution_;
};

}
