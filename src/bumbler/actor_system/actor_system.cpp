#include "actor_system.h"

namespace bumbler {

void actor_system::init() {
    actor_system_storage::instance().add_system(shared_from_this());

    work_ = utility::make_unique<boost::asio::io_service::work>(io_service_);

    server_ = std::make_shared<udp_server>(io_service_, port_);
    server_->send_completed_function = [&]() { send_completed(); };
    server_->receive_function = [&](std::unique_ptr<packet> packet,
                                    boost::asio::ip::udp::endpoint sender_endpoint) {
        receive(std::move(packet), sender_endpoint);
    };

    //create a future that is going to wait until the io_service thread has started and io_service is reading messages
    auto promise_ptr = std::make_shared<std::promise<int>>();
	auto f = promise_ptr->get_future();

    io_service_.post([promise_ptr] () {
        BOOST_LOG_TRIVIAL(debug) << "[IO_SERVICE] run thread started";
        promise_ptr->set_value(0);
    });

    io_service_thread_ = std::unique_ptr<std::thread>(new std::thread(
            [this]() {
                BOOST_LOG_TRIVIAL(debug) << "[ACTOR_SYSTEM_SERVER] STARTED ON PORT " << port_;
                io_service_.run();
            }
    ));

    auto status = f.wait_for(std::chrono::seconds(10));
    if(status != std::future_status::ready) {
        throw std::runtime_error("Couldn't start the actor server");
    }

    stopped_ = false;
    started_ = true;
}

void actor_system::stop(bool wait) {
    if (stopped_) return;

    for (auto iter = actors_.begin(); iter != actors_.end();) {
        iter->second->stop_actor(wait);
        actors_.erase(iter++);
    }

	scheduler_.reset();
	dispatcher_->stop(wait);
	actor_system_storage::instance().remove_system(system_name_);

    if(!started_) {
        server_.reset();
        return;
    }

    BOOST_LOG_TRIVIAL(debug) << "[ACTOR_SYSTEM] SHUTTING DOWN";
    server_->stop();
    started_ = false;
    stopped_ = true;

	if (work_) work_.reset();
    if (!io_service_.stopped()) io_service_.stop();
    if (io_service_thread_->joinable()) io_service_thread_->join();

#ifdef WIN32
    //workaround for windows
    //wait for the udp server to properly shutdown, otherwise it SIGSEGVs
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
    server_.reset();
}

std::shared_ptr<actor_system> actor_system::create_system(const std::string& name, int port, int thread_pool_size) {
    std::shared_ptr<actor_system> system = std::make_shared<concrete_actor_system>(name, port, thread_pool_size);
    system->init();

    return system;
}

int actor_system::stop_actor(std::string actor_name, bool wait) {
    if (stopped_.load()) throw new actor_system_stopped(system_name());
    std::lock_guard<std::mutex> guard(actors_write_mutex_);

    auto search = actors_.find(actor_name);
    if (search != actors_.end()) {
        actors_[actor_name]->stop_actor(wait);
        actors_.erase(search);
        return 0;
    }

    throw new actor_not_found(actor_name);
}

int actor_system::tell_actor(std::unique_ptr<message> msg, bool from_remote) {
    if (stopped_.load()) throw new actor_system_stopped(system_name());

    if (msg->get_target().system_name.compare(system_name_) != 0)
        throw new wrong_actor_system(msg->get_target().system_name);

    std::lock_guard<std::mutex> guard(actors_read_mutex_);
    std::string actor_name = msg->get_target().actor_name;

    auto search = actors_.find(actor_name);
    if (search != actors_.end()) {
        actors_[actor_name]->pass_message(std::move(msg), from_remote);
        return 0;
    }

    throw new actor_not_found(msg->get_target().actor_name);
}

int actor_system::ask_actor(std::unique_ptr<message> msg, const std::function<void(boost::any)>& response_fn) {
    if (stopped_.load()) throw new actor_system_stopped(system_name());

    if (msg->get_target().system_name.compare(system_name_) != 0)
        throw new wrong_actor_system(msg->get_target().system_name);

    std::lock_guard<std::mutex> guard(actors_read_mutex_);

    std::string actor_name = msg->get_target().actor_name;

    auto p = typed_props<promise_actor, typed_promise_actor>(response_fn);
    actor_ref p_actor = actor_of(p, get_next_temporary_actor_name());

    msg->set_sender(p_actor);

    auto search = actors_.find(actor_name);
    if (search != actors_.end()) {
        actors_[actor_name]->pass_message(std::move(msg), false);
        return 0;
    }

    throw new actor_not_found(msg->get_target().actor_name);
}

const actor_ref actor_system::get_actor_ref(std::string actor_name) {
    if (stopped_) nullptr;
    std::lock_guard<std::mutex> guard(actors_read_mutex_);

    auto search = actors_.find(actor_name);
    if (search != actors_.end()) {
        return actors_[actor_name]->get_self();
    }
    else {
        return actor_ref::none();
    }
}

const local_actor_channel actor_system::get_actor_channel(std::string actor_name) {
    if (stopped_) nullptr;

    return local_actor_channel();
}

void actor_system::receive(std::unique_ptr<packet> packet, boost::asio::ip::udp::endpoint& sender_endpoint) {
    std::stringstream ss(packet->data.data);
    boost::archive::text_iarchive ia(ss);

    std::unique_ptr<message> msg;
    ia >> msg;

    actor_ref sender = msg->get_sender();
    sender.ip = sender_endpoint.address().to_string();
    sender.port = sender_endpoint.port();
    msg->set_sender(sender);

    if (!msg->get_target().is_none()) {
        try {
            auto target_system = actor_system_storage::instance().get_system(msg->get_target().system_name);
            if(target_system != nullptr) {
                target_system->tell_actor(std::move(msg), true);
            }
        }
        catch (std::runtime_error& e) {
            BOOST_LOG_TRIVIAL(error) << "actor_system receive error: " << e.what();
        }
    }
    return;
}

std::string actor_system::get_next_temporary_actor_name() const {
    std::string temp_name = "temp/a";

    while(actors_.find(temp_name) != actors_.end()) {
        if(temp_name[temp_name.length() - 1] == 'z') {
            temp_name += 'a';
        }
        else {
            temp_name[temp_name.length() - 1]++;
        }
    }

    return temp_name;
}

int actor_system::add_actor(std::unique_ptr<abstract_actor> actor) {
    if (stopped_.load()) throw new actor_system_stopped(system_name());

    std::lock_guard<std::mutex> guard(actors_write_mutex_);
    auto search = actors_.find(actor->actor_name());
    if (search != actors_.end()) {
        throw new actor_already_exists(actor->actor_name());
    }

    actors_.emplace(actor->actor_name(), std::move(actor));
    return 0;
}

}