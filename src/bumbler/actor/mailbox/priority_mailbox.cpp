#include "priority_mailbox.h"

namespace bumbler {

std::unique_ptr<bumbler::message> priority_mailbox::pop_message() {
	std::unique_lock<std::mutex> lock(this->mailbox_mutex_);

	QueueElement moved = std::move(const_cast<QueueElement&>(this->queue_.top()));
	this->queue_.pop();

	return std::move(moved.msg);
}

std::vector<std::unique_ptr<bumbler::message>> priority_mailbox::pop_messages(size_t count) {
	std::unique_lock<std::mutex> lock(this->mailbox_mutex_);

	std::vector<std::unique_ptr<message>> result;
	result.reserve(count);
	for (size_t i = 0; i < count; i++) {
		if (this->queue_.empty()) break;

		QueueElement moved = std::move(const_cast<QueueElement&>(this->queue_.top()));
		this->queue_.pop();

		result.push_back(std::move(moved.msg));
	}

	return result;
}

void priority_mailbox::clear() {
	std::unique_lock<std::mutex> lock(this->mailbox_mutex_);
	while (!empty()) {
		this->queue_.pop();
	}
}

}