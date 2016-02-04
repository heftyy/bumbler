#include "abstract_actor.h"
#include "../actor_system/actor_system.h"

namespace bumbler {

abstract_actor::abstract_actor(const std::shared_ptr<actor_system>& actor_system, const std::string name) :
        actor_system_(actor_system),
        actor_name_(name),
        self_(actor_name(), system_name()),
        stop_flag_(false) { }

abstract_actor::~abstract_actor() {
    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] destructor";
    this->stop_actor();
}

void abstract_actor::init(std::unique_ptr<untyped_actor> u_actor) {
    this->untyped_actor_ = std::move(u_actor);
    this->untyped_actor_->set_self(this->get_self());
}

void abstract_actor::create_internal_queue_thread() {
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

std::string abstract_actor::actor_name() const {
    return this->actor_name_;
}

std::string abstract_actor::system_name() const {
    return actor_system_.lock()->system_name();
}

void abstract_actor::pass_message(std::unique_ptr<message> msg) {
    if(msg->is_kill_actor()) {
        this->actor_system_.lock()->stop_actor(this->actor_name_, false);
    }
    else if(msg->is_stop_actor()) {
        this->actor_system_.lock()->stop_actor(this->actor_name_, true);
    }
    else {
        this->tell(std::move(msg));
    }
}

void abstract_actor::stop_actor(bool wait) {
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

void abstract_actor::read_messages() {
    while (!this->mailbox_->empty()) {
        auto msg = this->mailbox_->pop_message();

        if(msg == nullptr) return;

        auto fun = [this] (const actor_ref& sender, const boost::any& data) -> int {
            this->run_task(sender, data);
            return 0;
        };

        // run the task in the thread pool supplied by the dispatcher
        auto future_result = this->actor_system_.lock()->get_dispatcher()->push(fun, msg->get_sender(), msg->get_data());

        // wait for the task to finish
        future_result.wait();
    }
}

void abstract_actor::add_message(std::unique_ptr<message> msg) {
    if(this->stop_flag_) return;

    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] queueing new task";
    this->mailbox_->push_message(std::move(msg));
    cv_.notify_one();
}

}
