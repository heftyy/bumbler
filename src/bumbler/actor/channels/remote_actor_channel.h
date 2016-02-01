#pragma once

#include <boost/asio.hpp>
#include "actor_channel.h"

namespace bumbler {

class actor_system;

class remote_actor_channel : public actor_channel {
public:
    remote_actor_channel() { }

    remote_actor_channel(const actor_ref& target_actor_ref) :
            actor_channel(target_actor_ref),
            network_actor_endpoint_(get_remote_endpoint(target_actor_ref)) { }
private:
    boost::asio::ip::udp::endpoint network_actor_endpoint_;

    virtual void tell_impl(std::unique_ptr<message> msg) override;
    virtual void ask_impl(std::unique_ptr<message> msg, const std::function<void(boost::any)>& response_fn) override;

    std::shared_ptr<actor_system> get_actor_system(const actor_ref& sender) const;
    boost::asio::ip::udp::endpoint get_remote_endpoint(const actor_ref& network_actor_ref) const;
};

}

