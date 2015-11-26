#pragma once

#include <memory>
#include <vector>
#include "../untyped_actor.h"
#include "../actor.h"
#include "router_logic.h"

class router_pool {
public:
    router_pool(router_pool&& rhs) = default; // support moving
    router_pool& operator=(router_pool&& rhs) = default;
    router_pool(const router_pool& rhs) : pool_size_(rhs.pool_size_), logic_(rhs.logic_), routees_() { }
    router_pool& operator=(const router_pool& rhs) {
        if(this != &rhs) {
            this->pool_size_ = rhs.pool_size_;
            this->logic_ = rhs.logic_;
        }
        return *this;
    }
    virtual ~router_pool() {}

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

    template<typename ActorType, typename TypedActorFunc>
    void create_pool(
            std::string router_name,
            const std::shared_ptr<actor_system>& actor_system,
            const std::unique_ptr<mailbox>& mbox,
            TypedActorFunc&& get_typed_actor_func) {
        for(int i = 0; i < this->pool_size_; i++) {
            auto actor = std::unique_ptr<ActorType>(new ActorType(router_name, actor_system));
            actor->set_mailbox(mbox->clone());
            actor->init(std::move(get_typed_actor_func()));
            this->routees_.emplace_back(std::move(actor));
        }
    }

    void stop(bool wait = false) {

    }

protected:
    router_pool(int pool_size, const router_logic &logic)
            : pool_size_(pool_size), logic_(logic) {
        assert(
                (pool_size > 0) &&
                "router pool size has to be greater than 0"
        );
    }

    int pool_size_;
    router_logic logic_;
    std::vector<std::unique_ptr<actor>> routees_;

    virtual void tell_one(std::unique_ptr<message> msg) = 0;

    void tell_all(std::unique_ptr<message> msg) {
        for(int i = 0; i < this->routees_.size(); i++) {
            auto msg_copy = std::move(msg->clone());

            routees_[i]->add_message(std::move(msg_copy));
        }
    }
};