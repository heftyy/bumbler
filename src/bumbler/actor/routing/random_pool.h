#pragma once

#include <vector>
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

    virtual std::unique_ptr<router_pool> clone() const override  {
        return std::unique_ptr<router_pool>(new random_pool(*this));
    }

protected:
    random_pool(const random_pool& rhs) :
            router_pool(rhs),
            distribution_(rhs.distribution_),
            random_gen_(rhs.random_gen_) { }

    random_pool& operator=(const random_pool& rhs) {
        if(this != &rhs) {
            router_pool::operator=(rhs);
            random_gen_ = rhs.random_gen_;
            distribution_ = rhs.distribution_;
        }
        return *this;
    }

private:
    std::random_device rd_;
    std::mt19937 random_gen_;
    std::uniform_int_distribution<> distribution_;
};

}
