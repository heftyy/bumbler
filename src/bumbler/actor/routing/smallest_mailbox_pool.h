#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include "router_pool.h"

namespace bumbler {

class smallest_mailbox_pool : public router_pool {
public:
    smallest_mailbox_pool(int pool_size)
            : router_pool(pool_size, router_logic::SMALLEST_MAILBOX_LOGIC) { }

    smallest_mailbox_pool(smallest_mailbox_pool&& rhs) = default; // support moving
    smallest_mailbox_pool& operator=(smallest_mailbox_pool&& rhs) = default;

    virtual ~smallest_mailbox_pool() {}

    void tell_one(std::unique_ptr<message> msg) override;

protected:
    smallest_mailbox_pool(const smallest_mailbox_pool& rhs) = delete;
    smallest_mailbox_pool& operator=(const smallest_mailbox_pool& rhs) = delete;

private:
    std::mutex mailbox_mutex_;
};

}
