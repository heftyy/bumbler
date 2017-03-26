#include "abstract_actor.h"
#include "untyped_actor.h"
#include "mailbox/mailbox.h"
#include "../messages/message.h"
#include "../actor_system/actor_system.h"

namespace bumbler {

abstract_actor::abstract_actor(const std::shared_ptr<actor_system>& actor_system, const std::string& name) :
        actor_system_(actor_system),
        actor_key_(name),
        self_(actor_key(), system_key()),
        stop_flag_(false) { }

abstract_actor::~abstract_actor() {
    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] destructor";
}

void abstract_actor::init(std::unique_ptr<untyped_actor> u_actor) {
    this->untyped_actor_ = std::move(u_actor);
    this->untyped_actor_->set_self(this->get_self());
}

size_t abstract_actor::mailbox_size() const { 
    return this->mailbox_->size(); 
}

identifier abstract_actor::actor_key() const {
    return this->actor_key_;
}

identifier abstract_actor::system_key() const {
    return actor_system_.lock()->system_key();
}

void abstract_actor::set_mailbox(std::unique_ptr<mailbox> mailbox) {
    this->mailbox_ = std::move(mailbox);
}

void abstract_actor::on_receive(const boost::any& data) {
    this->untyped_actor_->on_receive(data);
}

void abstract_actor::on_error(const boost::any& data, const std::exception & ex) {
    this->untyped_actor_->on_error(data, ex);
}

void abstract_actor::clear_queue() {
    if (this->mailbox_) this->mailbox_->clear();
}

}
