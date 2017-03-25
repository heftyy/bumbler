#pragma once

#include <memory>
#include <vector>
#include <cassert>
#include "router_logic.h"
#include "../../internal/bumbler.h"

namespace bumbler {

class actor_system;
class abstract_actor;
class message;

class router_pool {
public:
    router_pool(router_pool&& rhs) = default; // support moving
    router_pool& operator=(router_pool&& rhs) = default;

    virtual ~router_pool() {
        stop(stop_mode::IGNORE_QUEUE);
    }

    void tell(std::unique_ptr<message> msg);
    void stop(stop_mode stop_mode);

    template<typename ActorType, typename MailboxFunc, typename TypedActorFunc>
    void create(const std::shared_ptr<actor_system>& actor_system, const std::string& router_name,
                     MailboxFunc&& get_mailbox_func, TypedActorFunc&& get_typed_actor_func) {
        std::string next_router_name = router_name + "/a";

        for(int i = 0; i < this->pool_size_; i++) {
            auto actor = std::unique_ptr<ActorType>(new ActorType(actor_system, next_router_name));
            actor->set_mailbox(get_mailbox_func());
            actor->init(get_typed_actor_func());
            this->routees_.emplace_back(std::move(actor));

            if (next_router_name[next_router_name.length() - 1] == 'z') {
                next_router_name += 'a';
            }
            else {
                next_router_name[next_router_name.length() - 1]++;
            }
        }
    }

protected:
    int pool_size_;
    router_logic logic_;
    std::vector<std::unique_ptr<abstract_actor>> routees_;

    router_pool(int pool_size, const router_logic &logic) : pool_size_(pool_size), logic_(logic) {
        assert(pool_size > 0 && "router pool size has to be greater than 0");
    }

    router_pool(const router_pool& rhs) = delete;
    router_pool& operator=(const router_pool& rhs) = delete;

    virtual void tell_one(std::unique_ptr<message> msg) = 0;
    void tell_all(std::unique_ptr<message> msg);
};

}
