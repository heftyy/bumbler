#pragma once

#include "actor_channel.h"
#include "../../internal/identifier.h"

namespace bumbler {

class actor_system;

class local_actor_channel : public actor_channel {
public:
    local_actor_channel() : actor_channel() { }

    local_actor_channel(const identifier& actor, const std::shared_ptr<actor_system>& system) : actor_identifier_(actor), system_(system) {}

private:
    identifier actor_identifier_;
    std::weak_ptr<actor_system> system_;

    virtual bool expired() override;
    virtual void tell_impl(std::unique_ptr<message> msg) override;
    virtual void ask_impl(std::unique_ptr<message> msg, const ResponseFun& response_fun) override;
};

}

