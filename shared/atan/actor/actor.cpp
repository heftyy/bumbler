#include "actor.h"
#include "../actor_system/actor_system.h"

actor::actor(std::string name, const std::shared_ptr<actor_system>& actor_system)
        : actor_name_(name), actor_system_(actor_system) {
    stop_flag_.store(false);
    self_ = actor_ref(actor_name(), system_name());
}

actor::~actor() {
    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] destructor";
    this->stop_actor();
}

void actor::init(std::unique_ptr<untyped_actor> u_actor) {
    this->untyped_actor_ = std::move(u_actor);
    this->untyped_actor_->set_self(this->get_self());
    this->untyped_actor_->set_reply_message_function([this] (std::unique_ptr<message> msg) {
        this->send_reply_message(std::move(msg));
    });
}

void actor::create_internal_queue_thread() {
//    queue_thread_ = utility::make_unique<interruptible_thread>();
    queue_thread_future_ = std::async(std::launch::async ,[this]() {
        bool isPopped = false;

        while (true) {
            this->read_messages();

            std::unique_lock<std::mutex> lock(this->actor_thread_mutex_);

            this->cv.wait(lock, [this, &isPopped]() {
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
}

std::string actor::actor_name() {
    return this->actor_name_;
}

std::string actor::system_name() {
    return actor_system_.lock()->system_name();
}

void actor::send_reply_message(std::unique_ptr<message> msg) {
    if (stop_flag_.load()) return;

    //actor is not none and local
    if(!msg->get_target().is_none() && !msg->get_target().is_remote()) {
        auto ac = actor_system_storage::instance().get_system(msg->get_target().system_name);

        if(ac != nullptr) {
            actor_ref actor_from_system = ac->get_actor(msg->get_target().actor_name);
            if(!actor_from_system.is_none()) {
                ac->tell_actor(std::move(msg));
                return;
            }
        }
    }
    //actor is remote
    else if(!msg->get_target().is_none() && msg->get_target().is_remote()) {
        try {
            std::string ip = msg->get_target().ip;
            int port = msg->get_target().port;

            BOOST_LOG_TRIVIAL(debug) << "[ACTOR] replying to " << msg->get_target().to_string();
            packet p = message_to_packet(std::move(msg));

            auto remote_actor_endpoint = boost::asio::ip::udp::endpoint(
                    boost::asio::ip::address().from_string(ip), port
            );

            actor_system_.lock()->get_server()->do_send(p.get_serialized(), remote_actor_endpoint);
        }
        catch(std::exception e) {
            BOOST_LOG_TRIVIAL(error) << "[ACTOR] reply has failed: " << e.what();
        }
    }
}

void actor::pass_message(std::unique_ptr<message> msg, bool remote) {
    if(msg->is_kill_actor()) {
        this->actor_system_.lock()->stop_actor(this->actor_name_, false);
    }
    else if(msg->is_stop_actor()) {
        this->actor_system_.lock()->stop_actor(this->actor_name_, true);
    }
    else {
        this->tell(std::move(msg), remote);
    }
}

void actor::add_to_actor_system(const std::shared_ptr<actor_system>& system, std::unique_ptr<actor> actor_ptr) {
    system->add_actor(std::move(actor_ptr));
}

void actor::stop_actor(bool wait) {
    if(stop_flag_) return;

    stop_flag_ = true;

    if (!wait) clear_queue();

    if (queue_thread_future_.valid()) {
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopping";
        cv.notify_one();
        queue_thread_future_.wait();
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopped";
    }
}

void actor::read_messages() {
    while (message_queue_.size() > 0) {
        std::unique_lock<std::mutex> lock(this->message_queue_mutex_);

        auto msg = std::move(message_queue_.front());
        message_queue_.pop();

        lock.unlock();

        if(msg == nullptr) return;

        auto fun = [this] (int thread_id, const actor_ref& sender, const boost::any& data) -> int {
            this->run_task(sender, data);
            return 0;
        };

        //run the task in the thread pool supplied by the dispatcher
        auto future_result = this->actor_system_.lock()->get_dispatcher()->push(fun, msg->get_sender(), msg->get_data());

        future_result.wait();
    }
}

void actor::add_message(std::unique_ptr<message> msg) {
    if(this->stop_flag_) return;
    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] queueing new task";
    std::unique_lock<std::mutex> lock(this->message_queue_mutex_);
    message_queue_.push(std::move(msg));
    cv.notify_one();
}