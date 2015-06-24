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
    queue_thread_ = std::unique_ptr<interruptible_thread>(new interruptible_thread());
    queue_thread_->start([this]() {

        bool isPopped = false;

        while (true) {
            this->read_messages();

            std::unique_lock<std::mutex> lock(this->mutex_);

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

    this->actor_system_.lock()->add_actor(shared_from_this());
    return this->get_self();
}

std::string actor::actor_name() {
    return this->actor_name_;
}

std::string actor::system_name() {
    return actor_system_.lock()->system_name_;
}

void actor::send_reply_message(std::unique_ptr<message> msg) {
    auto ac = actor_system_storage::instance().get_system(msg->get_target().system_name);

    //actor is local
    if(ac != nullptr) {
        actor_ref actor_from_system = ac->get_actor(msg->get_target().actor_name);
        if(actor_from_system.exists()) {
            ac->tell_actor(std::move(msg));
            return;
        }
    }

    //actor is remote
    if(msg->get_target().valid_address()) {
        try {
            std::string ip = msg->get_target().ip;
            int port = msg->get_target().port;

            BOOST_LOG_TRIVIAL(debug) << "[ACTOR] replying to " << msg->get_target().to_string();
            packet p = message_to_packet(std::move(msg));

            auto remote_actor_endpoint = boost::asio::ip::udp::endpoint(
                    boost::asio::ip::address().from_string(ip), port
            );

            BOOST_LOG_TRIVIAL(debug) << "[ACTOR] reply message: " << p.get_serialized();
            actor_system_.lock()->get_server()->do_send(p.get_serialized(), remote_actor_endpoint);
        }
        catch(std::exception e) {
            BOOST_LOG_TRIVIAL(error) << "[ACTOR] reply has failed: " << e.what();
        }
    }
}