#pragma once

#include "../untyped_actor.h"
#include "../routing/router_pool.h"
#include "../mailbox/mailbox.h"

namespace bumbler {

class props {
public:
	props() :
		router_pool_set_(false),
		mailbox_set_(false),
		network_actor_ref_set_(false) { }

	virtual ~props() {}

	bool has_router() const {
        return router_pool_set_;
    }

    bool has_mailbox() const {
        return mailbox_set_;
    }

    bool has_network_actor() const {
        return network_actor_ref_set_;
    }

    virtual std::unique_ptr<abstract_actor> create_actor_instance(const std::shared_ptr<actor_system>& actor_system, const std::string name) = 0;

protected:
    bool router_pool_set_;
    bool mailbox_set_;
    bool network_actor_ref_set_;
};

}
