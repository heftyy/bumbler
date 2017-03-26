#pragma once

#include <memory>
#include <vector>
#include <cassert>
#include "router_logic.h"
#include "../../internal/bumbler.h"
#include "../../internal/identifier.h"

namespace bumbler {

class abstract_actor;
class message;
class actor_system;

class router_pool {
public:
    router_pool(router_pool&& rhs) = default; // support moving
    router_pool& operator=(router_pool&& rhs) = default;

    void tell(std::unique_ptr<message> msg);

    void create(const std::shared_ptr<actor_system>& actor_system, const std::string& router_name,
                const ActorCreateFun& get_actor_func, const MailboxCreateFun& get_mailbox_func, const TypedActorCreateFun& get_typed_actor_func);

protected:
    int pool_size_;
    router_logic logic_;
    std::vector<identifier> routees_;

    router_pool(int pool_size, const router_logic &logic) : pool_size_(pool_size), logic_(logic) {
        assert(pool_size > 0 && "router pool size has to be greater than 0");
    }

    router_pool(const router_pool& rhs) = delete;
    router_pool& operator=(const router_pool& rhs) = delete;

    virtual void tell_one(std::unique_ptr<message> msg) = 0;
    void tell_all(std::unique_ptr<message> msg);

    void send_message_to_routee(int routee_index, std::unique_ptr<message> msg);
};

}
