#include "random_router.h"

void random_router::tell_one(std::unique_ptr<message> msg) {

    int actor_number = this->distribution_(this->random_gen_);
    this->actors[actor_number]->add_message(std::move(msg));

	std::function<int(int)> f = [this, actor_number] (int thread_id) -> int {
		this->actors[actor_number]->read_messages();

		return 0;
	};

	this->thread_pool.push(f);
}