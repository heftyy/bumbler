#include "router_pool.h"
#include "../abstract_actor.h"
#include "../../messages/message.h"

void bumbler::router_pool::tell(std::unique_ptr<message> msg) {
	assert(routees_.size() > 0 && "router pool has to be initialized before you use tell");

	if (msg->is_broadcast()) {
		this->tell_all(std::move(msg));
	}
	else {
		this->tell_one(std::move(msg));
	}
}

void bumbler::router_pool::stop(bool wait) {
	for (auto&& routee : this->routees_) {
		routee->stop_actor(wait);
	}
}

void bumbler::router_pool::tell_all(std::unique_ptr<message> msg) {
	for (int i = 0; i < this->routees_.size(); i++) {
		auto msg_copy = std::move(msg->clone());

		routees_[i]->pass_message(std::move(msg_copy));
	}
}
