#pragma once

#include "actor_channel.h"

namespace bumbler {

class abstract_actor;

class local_actor_channel : public actor_channel {
public:
    local_actor_channel() : actor_channel() { }

    local_actor_channel(const std::shared_ptr<abstract_actor>& actor_ptr) :
            actor_ptr_(actor_ptr) { }

private:
    std::weak_ptr<abstract_actor> actor_ptr_;

    virtual bool expired() override;
    virtual void tell_impl(std::unique_ptr<message> msg) override;
    virtual void ask_impl(std::unique_ptr<message> msg, const ResponseFun& response_fun) override;
};

}

