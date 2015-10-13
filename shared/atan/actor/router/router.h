#pragma once

#include <list>
#include <memory>
#include <functional>
#include "../actor.h"
#include "../local_actor.h"

class actor_system;
class untyped_actor;

class router : public actor
{
public:
    void stop_actor(bool wait = false) override {
        actor::stop_actor(wait);

        for(int i = 0; i < size_; i++) {
            actors_[i]->stop_actor(wait);
        }
    }

protected:
    template<class T, typename Mailbox, typename ...Args>
    static actor_ref create(std::unique_ptr<router> router_ptr, const std::shared_ptr<actor_system>& actor_system, Args&& ...args) {
        static_assert(
                (std::is_base_of<untyped_actor, T>::value),
                "T has be a descendant of untyped_actor"
        );

        auto typed_actor = utility::make_unique<T>(std::forward<Args>(args)...);

        router_ptr->init(std::move(typed_actor));
        router_ptr->template create_actors<T, Mailbox>(std::forward<Args>(args)...);

	    auto& ar = router_ptr->get_self();
        actor::add_to_actor_system(actor_system, std::move(router_ptr));
        return ar;
    }

    int size_;
    std::vector<std::unique_ptr<actor>> actors_;

    router(const std::string& name, const std::shared_ptr<actor_system>& actor_system, int size)
            : actor(name, actor_system), size_(size) {
    }

    void init(std::unique_ptr<untyped_actor> u_actor) override;
    virtual void tell(std::unique_ptr<message> msg, bool remote = false) override;
    virtual void tell_one(std::unique_ptr<message> msg) = 0;
    void tell_all(std::unique_ptr<message> msg);

    template<typename T, typename Mailbox, typename ...Args>
    void create_actors(Args&& ...args) {
        for(int i = 0; i < size_; i++) {
	        auto actor = std::unique_ptr<local_actor>(new local_actor(this->actor_name(), this->actor_system_.lock()));
            auto typed_actor = utility::make_unique<T>(std::forward<Args>(args)...);
            actor->template setup_mailbox<Mailbox>();
            actor->init(std::move(typed_actor));
            this->actors_.push_back(std::move(actor));
        }
    }

    actor& get_actor(int i) {
        return *this->actors_[i];
    }
};

