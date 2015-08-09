#pragma once

#include <list>
#include <memory>
#include <functional>
#include "../actor.h"
#include "../local_actor.h"
#include "../../thread_pool/ctpl_stl.h"

class actor_system;
class untyped_actor;

class router : public actor
{
public:
    void stop_actor(bool wait = false) override {
        actor::stop_actor(wait);

        for(int i = 0; i < size; i++) {
            actors[i]->stop_actor(wait);
        }
    }

protected:
    template<class T, typename ...Args>
    static actor_ref create(std::unique_ptr<router> router_ptr, const std::shared_ptr<actor_system>& actor_system, Args&& ...args) {
        BOOST_STATIC_ASSERT_MSG(
                (std::is_base_of<untyped_actor, T>::value),
                "T has be a descendant of untyped_actor"
        );

        auto typed_actor = utility::make_unique<T>(args...);

        router_ptr->init(std::move(typed_actor));
        router_ptr->template create_actors<T>(args...);

	    auto& ar = router_ptr->get_self();
        actor::add_to_actor_system(actor_system, std::move(router_ptr));
        return ar;
    }

    int size;
    std::vector<std::unique_ptr<actor>> actors;

    router(const std::string& name, const std::shared_ptr<actor_system>& actor_system, int size)
            : actor(name, actor_system), size(size) {
    }

    void init(std::unique_ptr<untyped_actor> u_actor) override;
    virtual void tell(std::unique_ptr<message> msg, bool remote = false) override;
    virtual void tell_one(std::unique_ptr<message> msg) = 0;
    void tell_all(std::unique_ptr<message> msg);

    template<typename T, typename ...Args>
    void create_actors(Args&& ...args) {
        for(int i = 0; i < size; i++) {
	        auto actor = std::unique_ptr<local_actor>(new local_actor(this->actor_name(), this->actor_system_.lock()));
            auto typed_actor = utility::make_unique<T>(args...);
            actor->init(std::move(typed_actor));
            this->actors.push_back(std::move(actor));
        }
    }

    actor& get_actor(int i) {
        return *this->actors[i];
    }
};

