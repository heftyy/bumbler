#include "actor.h"
#include "../actor_system/actor_system.h"

actor::actor(const std::string name, std::weak_ptr<actor_system> actor_system)
        : actor_name_(name), actor_system_(actor_system) {
    stop_flag_.store(false);
}

actor::~actor() {
    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] destructor";
    this->stop_actor();
}

std::string actor::actor_name() {
    return this->actor_name_;
}

std::string actor::system_name() {
    return actor_system_.lock()->system_name_;
}

void actor::send_reply_message(std::unique_ptr<message> msg) {
    actor_ref actor_from_system = actor_system_.lock()->get_actor(msg->get_target().actor_name);
    if(actor_from_system.exists()) {
        actor_system_.lock()->tell_actor(std::move(msg));
    }
    else if(msg->get_target().valid_address()) {
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] replied to " << msg->get_target().to_string();
        packet p = message_to_packet(std::move(msg));
        auto remote_actor_endpoint = boost::asio::ip::udp::endpoint(
                boost::asio::ip::address().from_string(msg->get_target().ip), msg->get_target().port
        );
        actor_system_.lock()->get_server()->do_send(p.get_raw_packet(), remote_actor_endpoint);
    }
}