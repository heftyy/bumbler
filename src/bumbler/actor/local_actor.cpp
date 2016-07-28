#include "local_actor.h"
#include "../actor_system/actor_system.h"

namespace bumbler {

local_actor::~local_actor() {
    this->stop_actor();
}

void local_actor::init(std::unique_ptr<untyped_actor> u_actor) {
    abstract_actor::init(std::move(u_actor));
    this->create_internal_queue_thread();
}

void local_actor::stop_actor(bool wait) {
    if(stop_flag_) return;

    stop_flag_ = true;

    if (!wait) clear_queue();

    if (queue_thread_future_.valid()) {
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopping";
        cv_.notify_one();
        queue_thread_future_.wait();
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopped";
    }
}

void local_actor::tell(std::unique_ptr<message> msg) {
    add_message(std::move(msg));
}

void local_actor::create_internal_queue_thread() {
    queue_thread_future_ = std::async(std::launch::async ,[this]() {
        bool isPopped = false;

        while (true) {
            this->read_messages();

            std::unique_lock<std::mutex> lock(this->actor_thread_mutex_);

            this->cv_.wait(lock, [this, &isPopped]() {
                isPopped = !this->mailbox_->empty();
                if (isPopped) {
                    this->read_messages();
                }
                return isPopped || this->stop_flag_;
            });

            if (!isPopped)
                return;  // if the queue is empty and this->stop_flag_ is set to true
        }
    });
}

void local_actor::read_messages() {
    while (!this->mailbox_->empty()) {
        auto msg = this->mailbox_->pop_message();

        if(msg == nullptr) return;

        // run the task in the thread pool supplied by the dispatcher
        auto future_result = this->actor_system_.lock()->get_dispatcher()->push(dispatcher_fun_,
                                                                                msg->get_sender(),
                                                                                msg->get_data());

        // wait for the task to finish
        future_result.wait();
    }
}

void local_actor::add_message(std::unique_ptr<message> msg) {
    if(this->stop_flag_) return;

//    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] queueing new task";
    this->mailbox_->push_message(std::move(msg));
    cv_.notify_one();
}

void local_actor::run_task(const actor_ref& sender, const boost::any& data) {
    busy_ = true;
//    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] starting new task";
    this->untyped_actor_->set_sender(sender);

    try {
        on_receive(data);
    }
    catch(const std::exception& ex) {
        on_error(data, ex);
    }

    this->untyped_actor_->set_sender(actor_ref::none());
    busy_ = false;
}

}
