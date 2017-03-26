#pragma once

#include <memory>
#include "../local_actor.h"
#include "../promise_actor.h"
#include "../routing/router.h"
#include "../mailbox/fifo_mailbox.h"
#include "props.h"

namespace bumbler {

template<typename ActorType, typename TypedActor>
class typed_props : public props {
public:
    using DefaultMailbox = fifo_mailbox;

    template<typename ...Args>
    typed_props(Args&& ... args) : props() {
        static_assert(
                (std::is_base_of<untyped_actor, TypedActor>::value),
                "TypedActor template has to be a derived from untyped_actor class"
        );

        get_typed_actor_function_ = [args...]() -> std::unique_ptr<untyped_actor> {
            return std::make_unique<TypedActor>(std::forward<Args>(args)...);
        };
    }

    typed_props(typed_props&& rhs) = default; // support moving
    typed_props& operator=(typed_props&& rhs) = default;

    typed_props(const typed_props& rhs) = delete; // disable copying
    typed_props& operator=(const typed_props& rhs) = delete;

    template<typename RouterPool, typename ...RouterPoolArgs>
    typed_props& with_router(RouterPoolArgs&& ... pool_args) {
        get_router_pool_function_ = [pool_args...]() -> std::unique_ptr<router_pool> {
            return std::make_unique<RouterPool>(pool_args...);
        };
        router_pool_set_ = true;
        return *this;
    }

    template<typename Mailbox, typename ...MailboxArgs>
    typed_props& with_mailbox(MailboxArgs&& ... mailbox_args) {
        get_mailbox_function_ = [mailbox_args...]() -> std::unique_ptr<mailbox> {
            return std::make_unique<Mailbox>(mailbox_args...);
        };
        mailbox_set_ = true;
        return *this;
    }

    virtual std::shared_ptr<abstract_actor> create_actor_instance(const std::shared_ptr<actor_system>& actor_system,
                                                                  const std::string& name) override {
        if (!get_mailbox_function_) {
            get_mailbox_function_ = []() -> std::unique_ptr<mailbox> {
                return std::make_unique<DefaultMailbox>();
            };
        }

        return init_actor_instance(std::make_shared<ActorType>(actor_system, name),
                                   actor_system,
                                   name);
    }

private:
    std::function<std::unique_ptr<untyped_actor>(void)> get_typed_actor_function_;
    std::function<std::unique_ptr<router_pool>(void)> get_router_pool_function_;
    std::function<std::unique_ptr<mailbox>(void)> get_mailbox_function_;

    // router instance
    std::shared_ptr<abstract_actor> init_actor_instance(const std::shared_ptr<router>& actor,
                                                        const std::shared_ptr<actor_system>& ac,
                                                        const std::string& name) const {
        auto pool = get_router_pool_function_();
        pool->create<local_actor>(ac, name, get_mailbox_function_, get_typed_actor_function_);
        actor->set_router_pool(std::move(pool));

        return actor;
    }

    // promise actor instance
    std::shared_ptr<abstract_actor> init_actor_instance(const std::shared_ptr<promise_actor>& actor,
                                                        const std::shared_ptr<actor_system>& ac,
                                                        const std::string& name) const {
        actor->init(get_typed_actor_function_());
        return actor;
    }

    // local actor instance
    std::shared_ptr<abstract_actor> init_actor_instance(const std::shared_ptr<local_actor>& actor,
                                                        const std::shared_ptr<actor_system>& ac,
                                                        const std::string& name) const {
        actor->set_mailbox(get_mailbox_function_());
        actor->init(get_typed_actor_function_());

        return actor;
    }
};

}
