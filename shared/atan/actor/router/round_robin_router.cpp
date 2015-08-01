#include "round_robin_router.h"

void round_robin_router::tell_one(std::unique_ptr<message> msg) {

	std::function<int(std::shared_ptr<message>, int, int)> f = [this](std::shared_ptr<message>&& m, int actor_id, int id) -> int {
		BOOST_LOG_TRIVIAL(debug) << "CURRENT ACTOR IS = " << actor_id;
		this->actors[actor_id]->run_task(m->get_sender(), m->get_data());

		return 0;
	};

	std::function<int(int)> f_bound = std::bind(f, std::shared_ptr<message>(std::move(msg)), this->current_actor_to_message++, std::placeholders::_1);

	this->thread_pool.push(f_bound);

    if(this->current_actor_to_message >= this->size) {
        this->current_actor_to_message = 0;
    }
}