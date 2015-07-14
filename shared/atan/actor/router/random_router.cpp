#include "random_router.h"

void random_router::tell(std::unique_ptr<message> msg, bool remote) {

    this->thread_pool.push(
            std::bind(
                    [this] (std::unique_ptr<message>& m) {
                        int actor_number = this->distribution_(this->random_gen_);
                        this->actors[actor_number]->on_receive(m->get_data());
                    }, std::move(msg)
            )
    );

}