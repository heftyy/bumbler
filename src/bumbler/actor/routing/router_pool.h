#pragma once

#include <memory>
#include <vector>
#include "../abstract_actor.h"
#include "router_logic.h"

namespace bumbler {

class router_pool {
public:
    router_pool(router_pool&& rhs) = default; // support moving
    router_pool& operator=(router_pool&& rhs) = default;

    virtual ~router_pool() {
        stop();
    }

    virtual std::unique_ptr<router_pool> clone() const = 0;

    void tell(std::unique_ptr<message> msg) {
        assert(
                (routees_.size() > 0) &&
                "router pool has to be initialized before you use tell"
        );

        if(msg->is_broadcast()) {
            this->tell_all(std::move(msg));
        }
        else {
            this->tell_one(std::move(msg));
        }
    }

    template<typename ActorType, typename MailboxFunc, typename TypedActorFunc>
    void create_pool(const std::shared_ptr<actor_system>& actor_system,
                     std::string router_name,
                     MailboxFunc&& get_mailbox_func,
                     TypedActorFunc&& get_typed_actor_func) {
        for(int i = 0; i < this->pool_size_; i++) {
            auto actor = std::unique_ptr<ActorType>(new ActorType(actor_system, router_name));
            actor->set_mailbox(get_mailbox_func());
            actor->init(get_typed_actor_func());
            this->routees_.emplace_back(std::move(actor));
        }
    }

    void stop(bool wait = false) {
        for(auto& routee : this->routees_) {
            routee->stop_actor(wait);
        }
    }

protected:
    int pool_size_;
    router_logic logic_;
    std::vector<std::unique_ptr<abstract_actor>> routees_;

    router_pool(int pool_size, const router_logic &logic)
            : pool_size_(pool_size), logic_(logic) {
        assert(
                (pool_size > 0) &&
                "router pool size has to be greater than 0"
        );
    }

    router_pool(const router_pool& rhs) : pool_size_(rhs.pool_size_), logic_(rhs.logic_), routees_() { }
    router_pool& operator=(const router_pool& rhs) {
        if(this != &rhs) {
            this->pool_size_ = rhs.pool_size_;
            this->logic_ = rhs.logic_;
        }
        return *this;
    }

    virtual void tell_one(std::unique_ptr<message> msg) = 0;

    void tell_all(std::unique_ptr<message> msg) {
        for(int i = 0; i < this->routees_.size(); i++) {
            auto msg_copy = std::move(msg->clone());

            routees_[i]->pass_message(std::move(msg_copy));
        }
    }
};

}
