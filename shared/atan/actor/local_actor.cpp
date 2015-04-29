#include "local_actor.h"
#include "../actor_system/actor_system.h"

actor_ref local_actor::init()
{
    message_queue_ = std::queue<message>();
    queue_thread_ = std::unique_ptr<interruptible_thread>(new interruptible_thread());
    queue_thread_->start([this]() {
        this->read_messages();
    }, actor_sleep_ms_);

    this->actor_system_.lock()->add_actor(shared_from_this());
    return this->get_self();
}