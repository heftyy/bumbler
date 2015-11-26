#pragma once

#include <memory>
#include <utility.h>
#include "../local_actor.h"
#include "../routing/router.h"
#include "props.h"

template<typename ActorType, typename TypedActor, typename ...Args>
class typed_props : public props {
public:
    using DefaultMailbox = fifo_mailbox;

    typed_props(Args&& ...args) {
        static_assert(
                (std::is_base_of<untyped_actor, TypedActor>::value),
                "TypedActor template has to be a derived from untyped_actor class"
        );

        this->get_typed_actor_ = std::move([args...]() -> std::unique_ptr<untyped_actor> {
            return std::move(utility::make_unique<TypedActor>(args...));
        });
    }

    typed_props(typed_props &&rhs) = default; // support moving
    typed_props &operator=(typed_props &&rhs) = default;

    typed_props(const typed_props &rhs) = delete; // disable copying
    typed_props &operator=(const typed_props &rhs) = delete;

    std::unique_ptr<actor> create_actor_instance(const std::string name, const std::shared_ptr<actor_system>& actor_system) const override {
        auto actor = utility::make_unique<ActorType>(name, actor_system);

        std::cout << boost::typeindex::type_id_with_cvr<ActorType>().pretty_name() << std::endl;
        std::cout << boost::typeindex::type_id_with_cvr<decltype(actor)>().pretty_name() << std::endl;

        return init_actor_instance(std::move(actor), name, actor_system);
    }

private:
    std::function<std::unique_ptr<untyped_actor>()> get_typed_actor_;

    std::unique_ptr<actor> init_actor_instance(
            std::unique_ptr<router> actor,
            const std::string name,
            const std::shared_ptr<actor_system>& actor_system) const {
        if(has_router()) {
            auto pool = router_pool_->clone();
            pool->template create_pool<local_actor>(name, actor_system, mailbox_, get_typed_actor_);
            actor->set_router_pool(std::move(pool));
        }

        return std::move(actor);
    }

    std::unique_ptr<actor> init_actor_instance(
            std::unique_ptr<actor> actor,
            const std::string name,
            const std::shared_ptr<actor_system>& actor_system) const {
        if (has_mailbox()) actor->set_mailbox(mailbox_->clone());
        else actor->set_mailbox(utility::make_unique<DefaultMailbox>());

        actor->init(get_typed_actor_());

        return std::move(actor);
    }
};

