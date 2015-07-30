#pragma once

#include <future>
#include "actor.h"

class promise_actor : public actor {
public:
    friend class actor;

    promise_actor(const std::string name, std::shared_ptr<actor_system>& actor_system, std::function<void(boost::any)>& response_fn)
            : actor(name, actor_system), response_fn_(response_fn) {
    }

    ~promise_actor() {
        BOOST_LOG_TRIVIAL(debug) << "[PROMISE ACTOR] destructor";
    }

protected:

    void tell(std::unique_ptr<message> msg, bool remote = false) override {
        on_receive(msg->get_data());
    }

    void on_receive(boost::any data) override;

private:
    std::function<void(boost::any)> response_fn_;

};
