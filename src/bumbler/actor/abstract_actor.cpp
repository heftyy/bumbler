#include "abstract_actor.h"
#include "../actor_system/actor_system.h"

namespace bumbler {

abstract_actor::abstract_actor(const std::shared_ptr<actor_system>& actor_system, const std::string& name) :
        actor_system_(actor_system),
        actor_name_(name),
        self_(actor_name(), system_name()),
        stop_flag_(false) { }

abstract_actor::~abstract_actor() {
    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] destructor";
}

void abstract_actor::init(std::unique_ptr<untyped_actor> u_actor) {
    this->untyped_actor_ = std::move(u_actor);
    this->untyped_actor_->set_self(this->get_self());
}

std::string abstract_actor::actor_name() const {
    return this->actor_name_;
}

std::string abstract_actor::system_name() const {
    return actor_system_.lock()->system_name();
}

void abstract_actor::pass_message(std::unique_ptr<message> msg) {
    if(msg->is_kill_actor()) {
        this->actor_system_.lock()->stop_actor(this->actor_name_, false);
    }
    else if(msg->is_stop_actor()) {
        this->actor_system_.lock()->stop_actor(this->actor_name_, true);
    }
    else {
        this->tell(std::move(msg));
    }
}

}
