#include "random_router.h"
#include "../../actor_system/actor_system.h"

actor_ref random_router::init() {
    this->actor_system_.lock()->add_actor(shared_from_this());
    return this->get_self();
}

void random_router::tell(std::unique_ptr<message> msg, bool remote) {

    this->thread_pool_.push(
            std::bind(
                    [this] (std::unique_ptr<message>& m) {
                        this->on_receive(m->get_data());
                    }, std::move(msg)
            )
    );

}