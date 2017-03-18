#include "abstract_actor.h"
#include "untyped_actor.h"
#include "mailbox/mailbox.h"
#include "../messages/message.h"
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

void abstract_actor::pass_message(std::unique_ptr<message> msg) {
	if (msg->is_kill_actor()) {
		this->actor_system_.lock()->stop_actor(this->actor_name_, stop_mode::IGNORE_QUEUE);
	}
	else if (msg->is_stop_actor()) {
		this->actor_system_.lock()->stop_actor(this->actor_name_, stop_mode::WAIT_FOR_QUEUE);
	}
	else {
		this->tell(std::move(msg));
	}
}

size_t abstract_actor::mailbox_size() const { 
	return this->mailbox_->size(); 
}

std::string abstract_actor::actor_name() const {
    return this->actor_name_;
}

std::string abstract_actor::system_name() const {
    return actor_system_.lock()->system_name();
}

void abstract_actor::set_mailbox(std::unique_ptr<mailbox> mbox) {
	this->mailbox_ = std::move(mbox);
}

void abstract_actor::on_receive(const boost::any & data) {
	this->untyped_actor_->on_receive(data);
}

void abstract_actor::on_error(const boost::any & data, const std::exception & ex) {
	this->untyped_actor_->on_error(data, ex);
}

void abstract_actor::clear_queue() {
	if (this->mailbox_) this->mailbox_->clear();
}

}
