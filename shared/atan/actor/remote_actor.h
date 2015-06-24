#pragma once

#include <memory>
#include <boost/asio.hpp>
#include <boost/any.hpp>
#include "actor.h"
#include "actor_ref.h"
#include "../packet/packet.h"
#include "../messages/serialization_utility.h"

class remote_actor : public actor {
protected:
    remote_actor(const std::string& name, std::shared_ptr<actor_system>& actor_system, const actor_ref& network_actor_ref)
            : actor(name, actor_system), network_actor_ref_(network_actor_ref) {
        remote_actor_endpoint_ = boost::asio::ip::udp::endpoint(
                boost::asio::ip::address().from_string(network_actor_ref.ip), network_actor_ref.port);
    }

    void tell(std::unique_ptr<message> msg, bool remote = false);

private:
    boost::asio::ip::udp::endpoint remote_actor_endpoint_;
    actor_ref network_actor_ref_;

    void tell_(packet& p);

    /*
    template<typename T>
	boost::any future_(message<T>& msg, int timeout_ms);
    */
};