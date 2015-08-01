#include "random_router.h"

void random_router::tell_one(std::unique_ptr<message> msg) {

	std::function<int(std::shared_ptr<message>, int)> f = [this] (std::shared_ptr<message>&& m, int id) -> int {
		int actor_number = this->distribution_(this->random_gen_);
		this->actors[actor_number]->run_task(m->get_sender(), m->get_data());

		return 0;
	};

	std::function<int(int)> bound_f = std::bind(f, std::shared_ptr<message>(std::move(msg)), std::placeholders::_1);

	this->thread_pool.push(bound_f);

}