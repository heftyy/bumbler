#pragma once

#include "actor_channel.h"

namespace bumbler {

class local_actor_channel : public actor_channel {
public:
    local_actor_channel() : actor_channel() { }

    local_actor_channel(const actor_ref& actor_ref_, const std::shared_ptr<abstract_actor>& actor_ptr_) :
            actor_channel(actor_ref_, actor_ptr_) { }

private:
    virtual void tell_impl(std::unique_ptr<message> msg) const;

    virtual void ask_impl(std::unique_ptr<message> msg, const std::function<void(boost::any)>& response_fn) const;
};

}

