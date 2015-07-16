#include "random_router.h"

void random_router::tell_one(std::unique_ptr<message> msg) {

    this->thread_pool.push(
            std::bind(
                    [this] (std::unique_ptr<message>& m) {
                        int actor_number = this->distribution_(this->random_gen_);
                        this->actors[actor_number]->run_task(m->get_sender(), m->get_data());
                    }, std::move(msg)
            )
    );

}