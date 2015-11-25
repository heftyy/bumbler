#pragma once

#include <memory>
#include <vector>
#include "../untyped_actor.h"
#include "../actor.h"
#include "router_logic.h"

class router_pool {
public:
    router_pool(router_pool&& rhs) = default;
    router_pool(const router_pool& rhs) = default;
    router_pool& operator=(router_pool&& rhs) = default;
    router_pool& operator=(const router_pool& rhs) = default;
    virtual ~router_pool() {}

    virtual std::unique_ptr<router_pool> clone() { }

    void tell(std::unique_ptr<message> msg) {
        if(msg->is_broadcast()) {
            this->tell_all(std::move(msg));
        }
        else {
            this->tell_one(std::move(msg));
        }
    }

    template<typename ActorType>
    void create_pool(
            std::string router_name,
            const std::shared_ptr<actor_system>& actor_system,
            std::function<std::unique_ptr<untyped_actor>()>& get_typed_actor,
            const std::unique_ptr<mailbox>& mbox) {
        auto actor = std::unique_ptr<ActorType>(new ActorType(router_name, actor_system));
        actor->set_mailbox(mbox->clone());
        actor->init(std::move(get_typed_actor()));
        this->routees_.push_back(std::move(actor));
    };

protected:
    router_pool(int pool_size, const router_logic &logic)
            : pool_size_(pool_size), logic_(logic) { }

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