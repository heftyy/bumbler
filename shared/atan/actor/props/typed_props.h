#pragma once

#include <memory>
#include <utility.h>
#include <boost/any.hpp>
#include "../local_actor.h"
#include "../remote_actor.h"
#include "../promise_actor.h"
#include "../routing/router.h"
#include "props.h"

template<typename ActorType, typename TypedActor>
class typed_props : public props {
public:
    using DefaultMailbox = fifo_mailbox;

    template<typename ...Args>
    typed_props(Args&&... args) {
        static_assert(
                (std::is_base_of<untyped_actor, TypedActor>::value),
                "TypedActor template has to be a derived from untyped_actor class"
        );

        this->get_typed_actor_ = [args...]() -> std::unique_ptr<untyped_actor> {
            return utility::make_unique<TypedActor>(args...);
        };
    }

    typed_props(typed_props &&rhs) = default; // support moving
    typed_props &operator=(typed_props &&rhs) = default;

    typed_props(const typed_props &rhs) = delete; // disable copying
    typed_props &operator=(const typed_props &rhs) = delete;

    template<typename RouterPool, typename ...RouterPoolArgs>
    typed_props& with_router(RouterPoolArgs&&... pool_args) {
        this->get_router_pool_ = [pool_args...]() -> std::unique_ptr<router_pool> {
            return utility::make_unique<RouterPool>(pool_args...);
        };
        this->router_pool_set_ = true;
        return *this;
    }

    template<typename Mailbox, typename ...MailboxArgs>
    typed_props& with_mailbox(MailboxArgs&&... mailbox_args) {
        this->get_mailbox_ = [mailbox_args...]() -> std::unique_ptr<mailbox> {
            return utility::make_unique<Mailbox>(mailbox_args...);
        };
        this->mailbox_set_ = true;
        return *this;
    }

    typed_props& with_network_actor(const std::string network_actor_ref) {
        return with_network_actor(actor_ref(network_actor_ref));
    }

    typed_props& with_network_actor(const actor_ref& network_actor_ref) {
        this->network_actor_ref_ = network_actor_ref;
        this->network_actor_ref_set_ = true;
        return *this;
    }

    std::unique_ptr<actor> create_actor_instance(const std::shared_ptr<actor_system>& actor_system,
                                                 const std::string name) const override {
        return init_actor_instance(std::move(utility::make_unique<ActorType>(actor_system, name)),
                                   actor_system,
                                   name);

    }

private:
    std::function<std::unique_ptr<untyped_actor>()> get_typed_actor_;
    std::function<std::unique_ptr<router_pool>()> get_router_pool_;
    std::function<std::unique_ptr<mailbox>()> get_mailbox_;
    actor_ref network_actor_ref_;

    // router instance
    std::unique_ptr<actor> init_actor_instance(std::unique_ptr<router>&& actor,
                                               const std::shared_ptr<actor_system>& actor_system,
                                               const std::string name) const {
        auto pool = get_router_pool_();
        pool->template create_pool<local_actor>(actor_system, name, get_mailbox_, get_typed_actor_);
        actor->set_router_pool(std::move(pool));

        return std::move(actor);
    }

    // remote actor instance
    std::unique_ptr<actor> init_actor_instance(std::unique_ptr<remote_actor>&& actor,
                                               const std::shared_ptr<actor_system>& actor_system,
                                               const std::string name) const {
        if (has_mailbox()) actor->set_mailbox(get_mailbox_());
        else actor->set_mailbox(std::move(utility::make_unique<DefaultMailbox>()));

        actor->init(get_typed_actor_());
        actor->set_network__actor_ref(network_actor_ref_);

        return std::move(actor);
    }

    // promise actor instance
    std::unique_ptr<actor> init_actor_instance(std::unique_ptr<promise_actor>&& actor,
                                               const std::shared_ptr<actor_system>& actor_system,
                                               const std::string name) const {
        actor->init(get_typed_actor_());
        return std::move(actor);
    }

    // local actor instance
    std::unique_ptr<actor> init_actor_instance(std::unique_ptr<local_actor>&& actor,
                                               const std::shared_ptr<actor_system>& actor_system,
                                               const std::string name) const {
        if (has_mailbox()) actor->set_mailbox(get_mailbox_());
        else actor->set_mailbox(std::move(utility::make_unique<DefaultMailbox>()));

        actor->init(get_typed_actor_());

        return std::move(actor);
    }
};

