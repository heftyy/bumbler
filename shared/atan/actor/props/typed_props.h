#pragma once

#include <memory>
#include <utility.h>
#include "props.h"

template<typename ActorType, typename TypedActor, typename ...Args>
class typed_props : public props {
public:
    typed_props(Args&& ...args) {
        static_assert(
                (std::is_base_of<untyped_actor, TypedActor>::value),
                "TypedActor template has to be a derived from untyped_actor class"
        );

        this->get_typed_actor_ = std::move([args...]() -> std::unique_ptr<untyped_actor> {
            return std::move(utility::make_unique<TypedActor>(std::forward<Args>(args)...));
        });
    }

    typed_props(typed_props &&rhs) = default; // support moving
    typed_props &operator=(typed_props &&rhs) = default;

    typed_props(const typed_props &rhs) = delete; // disable copying
    typed_props &operator=(const typed_props &rhs) = delete;

    std::unique_ptr<actor> create_actor_instance(std::string name, const std::shared_ptr<actor_system>& actor_system) override {
        auto actor = utility::make_unique<ActorType>(name, actor_system);

        if(has_mailbox()) actor->set_mailbox(mailbox_->clone());
        if(has_router()) {
            auto pool = router_pool_->clone();
            pool->template create_pool<ActorType>(name, actor_system, get_typed_actor_, mailbox_);
            actor->set_router_pool(router_pool_->clone());
        }

        return std::move(actor);
    }

    bool has_router() const {
        return router_pool_ != nullptr;
    }

    bool has_mailbox() const {
        return mailbox_ != nullptr;
    }

private:
    std::function<std::unique_ptr<untyped_actor>()> get_typed_actor_;
};

