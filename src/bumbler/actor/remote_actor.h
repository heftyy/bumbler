#pragma once

#include <memory>
#include <boost/asio.hpp>
#include <boost/any.hpp>
#include "abstract_actor.h"
#include "actor_ref/actor_ref.h"
#include "../packet/packet.h"

class actor_system;
class untyped_actor;

class remote_actor : public abstract_actor {
public:
    remote_actor(const std::shared_ptr<actor_system>& actor_system, const std::string name)
            : abstract_actor(actor_system, name) { }

    void init(std::unique_ptr<untyped_actor> u_actor) override;
    void set_network_actor_ref(const actor_ref& network_actor_ref) {
        this->network_actor_ref_ = network_actor_ref;
        this->network_actor_endpoint_ = boost::asio::ip::udp::endpoint(
                boost::asio::ip::address().from_string(network_actor_ref.ip),
                static_cast<unsigned short>(network_actor_ref.port)
        );
    }

protected:
    void tell(std::unique_ptr<message> msg, bool remote = false) override;

private:
    boost::asio::ip::udp::endpoint network_actor_endpoint_;
    actor_ref network_actor_ref_;

    void tell_(packet& p);
};