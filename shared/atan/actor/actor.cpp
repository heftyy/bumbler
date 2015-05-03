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