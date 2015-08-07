#pragma once

#include "actor.h"
#include "../actor_system/actor_system_errors.h"

class actor_system;
class untyped_actor;

class local_actor : public actor {
public:
    friend class router;

    template<class T, typename ...Args>
    static actor_ref create(const std::string& name, const std::shared_ptr<actor_system>& actor_system, Args&& ...args) {
        BOOST_STATIC_ASSERT_MSG(
                (std::is_base_of<untyped_actor, T>::value),
                "T has be a descendant of untyped_actor"
        );

        std::unique_ptr<local_actor> actor = std::unique_ptr<local_actor>(new local_actor(name, actor_system));
        std::unique_ptr<T> typed_actor = utility::make_unique<T>(args...);
        actor->init(std::move(typed_actor));
        actor_ref& ar = actor->get_self();
        actor::add_to_actor_system(actor_system, std::move(actor));
        return ar;
    }

protected:
    friend class actor;

    local_actor(const std::string& name, const std::shared_ptr<actor_system>& actor_system)
            : actor(name, actor_system) {
    }

    void init(std::unique_ptr<untyped_actor> u_actor) override;

    void tell(std::unique_ptr<message> msg, bool remote = false) override;
};
