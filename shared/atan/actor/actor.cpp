#include "actor.h"
#include "../actor_system/actor_system.h"

actor::actor(const std::string name, std::weak_ptr<actor_system> actor_system)
        : actor_name_(name), actor_system_(actor_system) {
    stop_flag_.store(false);
    self_ = actor_ref(actor_name(), system_name());
}

actor::~actor() {
    BOOST_LOG_TRIVIAL(debug) << "[ACTOR] destructor";
    this->stop_actor();
}

actor_ref actor::init() {
    return this->get_self();
}

void actor::create_internal_queue_thread() {
    queue_thread_ = utility::make_unique<interruptible_thread>();
    queue_thread_->start([this]() {

        bool isPopped = false;

        while (true) {
            BOOST_LOG_TRIVIAL(debug) << "internal thread loop start";

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
    else if(msg->get_target().is_remote()) {
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

    if (queue_thread_) {
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopping";
        cv.notify_one();
        queue_thread_->stop();
        queue_thread_.release();
        BOOST_LOG_TRIVIAL(debug) << "[ACTOR] stopped";
    }
}