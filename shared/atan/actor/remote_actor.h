#pragma once

#include <memory>
#include <boost/asio.hpp>
#include <boost/any.hpp>
#include "actor.h"
#include "actor_ref/actor_ref.h"
#include "../packet/packet.h"

class actor_system;
class untyped_actor;

class remote_actor : public actor {
public:
    template<class T, typename ...Args>
    static actor_ref create(const std::string& name, const std::shared_ptr<actor_system>& actor_system, const actor_ref& network_actor_ref, Args&& ...args) {
        BOOST_STATIC_ASSERT_MSG(
                (std::is_base_of<untyped_actor, T>::value),
                "T has be a descendant of untyped_actor"
        );

	    auto actor = std::unique_ptr<remote_actor>(new remote_actor(name, actor_system, network_actor_ref));
        auto typed_actor = utility::make_unique<T>(args...);
        actor->init(std::move(typed_actor));
	    auto& ar = actor->get_self();
        actor::add_to_actor_system(actor_system, std::move(actor));
        return ar;
    }

protected:
    friend class actor;

    remote_actor(const std::string& name, const std::shared_ptr<actor_system>& actor_system, const actor_ref& network_actor_ref)
            : actor(name, actor_system), network_actor_ref_(network_actor_ref) {
        remote_actor_endpoint_ = boost::asio::ip::udp::endpoint(
                boost::asio::ip::address().from_string(network_actor_ref.ip), network_actor_ref.port);
    }

    void init(std::unique_ptr<untyped_actor> u_actor) override;

    void tell(std::unique_ptr<message> msg, bool remote = false) override;

private:
    boost::asio::ip::udp::endpoint remote_actor_endpoint_;
    actor_ref network_actor_ref_;

    void tell_(packet& p);
};