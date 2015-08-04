#include "router.h"
#include "../../actor_system/actor_system.h"

actor_ref router::init() {
    this->thread_pool.resize(size);
    return actor::init();
}

void router::tell(std::unique_ptr<message> msg, bool remote) {
    if(msg->is_broadcast()) {
        this->tell_all(std::move(msg));
    }
    else {
        this->tell_one(std::move(msg));
    }
}

void router::tell_all(std::unique_ptr<message> msg) {
    std::shared_ptr<message> shared_message(std::move(msg));

    std::function<int(std::shared_ptr<message>, int, int)> f = [this] (std::shared_ptr<message>&& m, int actor_id, int thread_id) -> int {
        BOOST_LOG_TRIVIAL(debug) << "CURRENT ACTOR IS = " << actor_id;
        this->actors[actor_id]->run_task(m->get_sender(), m->get_data());

        return 0;
    };

    for(int i = 0; i < this->actors.size(); i++) {
        std::function<int(int)> bound_f = std::bind(f, shared_message, i, std::placeholders::_1);

        this->thread_pool.push(bound_f);
    }
}