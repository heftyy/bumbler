#include "typed_message.h"

bumbler::typed_message::typed_message(const actor_ref& target, const actor_ref& sender,
									  std::unique_ptr<variant> variant)
	: target_(target), sender_(sender), variant_(std::move(variant)) {

}

bumbler::typed_message::typed_message(const typed_message& rhs) : message(rhs) {
	std::cout << "typed_message copy ctor\n";
	this->variant_ = rhs.variant_->clone();
	this->target_ = rhs.target_;
	this->sender_ = rhs.sender_;
}

bumbler::typed_message& bumbler::typed_message::operator=(const typed_message& rhs) {
	std::cout << "typed_message assign ctor\n";
	this->variant_ = rhs.variant_->clone();
	this->target_ = rhs.target_;
	this->sender_ = rhs.sender_;
	return *this;
}

int bumbler::typed_message::get_priority() const {
	if (is_priority_message()) {
		priority_message pm = boost::any_cast<priority_message>(variant_->data());
		return pm.priority;
	}

	return 0;
}

boost::any bumbler::typed_message::get_data() const {
	if (is_broadcast()) {
		broadcast cmd = boost::any_cast<broadcast>(variant_->data());
		return cmd.var->data();
	}
	if (is_stop_actor()) {
		stop_actor cmd = boost::any_cast<stop_actor>(variant_->data());
		return cmd.var->data();
	}
	if (is_kill_actor()) {
		kill_actor cmd = boost::any_cast<kill_actor>(variant_->data());
		return cmd.var->data();
	}
	if (is_priority_message()) {
		priority_message cmd = boost::any_cast<priority_message>(variant_->data());
		return cmd.var->data();
	}

	return variant_->data();
}
