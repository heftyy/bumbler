#pragma once

#include <future>
#include "actor.h"

class actor_system;
class untyped_actor;

class promise_actor : public actor {
public:
    friend class actor;

    static actor_ref create(const std::string& name, const std::shared_ptr<actor_system> actor_system_ptr, std::function<void(boost::any)>& response_fn) {
	    auto actor = std::unique_ptr<promise_actor>(new promise_actor(name, actor_system_ptr, response_fn));
	    auto& ar = actor->get_self();
        actor::add_to_actor_system(actor_system_ptr, std::move(actor));
        return ar;
    }

protected:
    promise_actor(const std::string name, std::shared_ptr<actor_system> actor_system, std::function<void(boost::any)>& response_fn)
            : actor(name, actor_system), response_fn_(response_fn) {
    }

    void tell(std::unique_ptr<message> msg, bool remote = false) override {
        on_receive(msg->get_data());
    }

    void on_receive(boost::any data) override;

private:
    std::function<void(boost::any)> response_fn_;

};
