#include "local_actor.h"
#include "../actor_system/actor_system.h"

actor_ref local_actor::init()
{
    message_queue_ = std::queue<message>();
    queue_thread_ = std::unique_ptr<interruptible_thread>(new interruptible_thread());
    queue_thread_->start([this]() {

        bool isPopped = false;

        while(true)
        {
            this->read_messages();

            std::unique_lock<std::mutex> lock(this->mutex_);

            this->cv.wait(lock, [this, &isPopped](){
                isPopped = !this->message_queue_.empty();
                if (isPopped) {
                    this->read_messages();
                }
                return isPopped || this->stop_flag_;
            });

            if (!isPopped)
                return;  // if the queue is empty and this->stop_flag_ is set to true
        }
    });

    this->actor_system_.lock()->add_actor(shared_from_this());
    return this->get_self();
}