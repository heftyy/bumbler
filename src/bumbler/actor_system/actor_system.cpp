#include "actor_system.h"
#include "udp_server.h"
#include "actor_system_errors.h"
#include "actor_system_storage.h"
#include "../dispatcher/dispatcher.h"
#include "../scheduler/scheduler.h"
#include "../messages/typed_message.h"
#include "../actor/actor_ref/actor_ref.h"
#include "../actor/typed_promise_actor.h"
#include "../actor/channels/local_actor_channel.h"
#include "../actor/abstract_actor.h"
#include "../actor/promise_actor.h"
#include "../packet/packet.h"
#include "../actor/props/typed_props.h"

namespace bumbler {

actor_system::actor_system(const std::string& name, int port)
    : port_(port), system_key_(name) {}

actor_system::~actor_system() {
    stop();
}

void actor_system::init(int thread_pool_size) {
    dispatcher_ = std::make_unique<dispatcher>(thread_pool_size);
    scheduler_ = std::make_unique<scheduler>();

    work_ = std::make_unique<boost::asio::io_service::work>(io_service_);

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

    actor_system_storage::instance().register_system(shared_from_this());
}

void actor_system::stop(stop_mode stop_mode) {
    if (stopped_) return;

    for (auto iter = actors_.begin(); iter != actors_.end();) {
        iter->second->stop_actor(stop_mode);
        actors_.erase(iter++);
    }

    scheduler_.reset();
    dispatcher_->stop(stop_mode);
    actor_system_storage::instance().remove_system(system_key_);

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
    // workaround for windows
    // wait for the udp server to properly shutdown, otherwise it SIGSEGVs
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
    server_.reset();
}

std::shared_ptr<actor_system> actor_system::create_system(const std::string& name, int port, int thread_pool_size) {
    std::shared_ptr<actor_system> system = std::make_shared<actor_system>(name, port);
    system->init(thread_pool_size);

    return system;
}

int actor_system::stop_actor(const identifier& actor_ident, stop_mode stop_mode) {
    if (stopped_.load()) throw new actor_system_stopped(system_key_.to_string());
    std::lock_guard<std::mutex> guard(actors_write_mutex_);

    auto search = actors_.find(actor_ident);
    if (search != actors_.end()) {
        search->second->stop_actor(stop_mode);
        actors_.erase(search);
        return 0;
    }

    throw new actor_not_found(actor_ident.to_string());
}

void actor_system::tell_actor(std::unique_ptr<message> msg) {
    if (stopped_.load()) throw new actor_system_stopped(system_key_.to_string());

    if (msg->get_target().system_key != system_key_)
        throw new wrong_actor_system(msg->get_target().system_key.to_string());

    identifier actor_key = msg->get_target().actor_key;

    if (msg->is_kill_actor()) {
        stop_actor(actor_key, stop_mode::IGNORE_QUEUE);
    }
    else if (msg->is_stop_actor()) {
        stop_actor(actor_key, stop_mode::WAIT_FOR_QUEUE);
    }
    else {
        auto search = actors_.find(actor_key);
        if (search != actors_.end()) {
            search->second->tell(std::move(msg));
        }
        else {
            throw new actor_not_found(msg->get_target().actor_key.to_string());
        }
    }
}
#if 0
void actor_system::ask_actor(std::unique_ptr<message> msg, const ResponseFun& response_fn) {
    if (stopped_.load()) throw new actor_system_stopped(system_name());

    if (msg->get_target().system_name.compare(system_name_) != 0)
        throw new wrong_actor_system(msg->get_target().system_name);

    std::string actor_ident = msg->get_target().actor_ident;

    auto p = typed_props<promise_actor, typed_promise_actor>(response_fn);
    actor_ref p_actor = actor_of(p, get_next_temporary_actor_name());

    msg->set_sender(p_actor);

    if (msg->is_kill_actor()) {
        stop_actor(actor_key, stop_mode::IGNORE_QUEUE);
    }
    else if (msg->is_stop_actor()) {
        stop_actor(actor_key, stop_mode::WAIT_FOR_QUEUE);
    }
    else {
        auto search = actors_.find(actor_key);
        if (search != actors_.end()) {
            search->second->tell(std::move(msg));
        }
        else {
            throw new actor_not_found(msg->get_target().actor_key.to_string());
        }
    }
}
#endif

bool actor_system::has_actor(const identifier& actor_ident)
{
    auto search = actors_.find(actor_ident);
    return search != actors_.end();
}

actor_ref actor_system::get_actor_ref(const identifier& actor_ident) {
    if (stopped_) nullptr;

    auto search = actors_.find(actor_ident);
    if (search != actors_.end()) {
        return search->second->get_self();
    }
    else {
        return actor_ref::none();
    }
}

std::unique_ptr<actor_channel> actor_system::get_actor_channel(const identifier& actor_ident) {
    if (stopped_) nullptr;

    auto search = actors_.find(actor_ident);
    if (search != actors_.end()) {
        return std::make_unique<local_actor_channel>(search->first, shared_from_this());
    }

    return nullptr;
}

size_t actor_system::actor_mailbox_size(const identifier& actor_ident) {
    if (stopped_) return 0;

    auto search = actors_.find(actor_ident);
    if (search != actors_.end()) {
        return search->second->mailbox_size();
    }

    return 0;
}

void actor_system::receive(std::unique_ptr<packet> packet, const boost::asio::ip::udp::endpoint& sender_endpoint) const {
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
            auto target = msg->get_target();
            auto target_system = actor_system_storage::instance().get_system(target.system_key);
            if(target_system != nullptr) {
                target_system->tell_actor(std::move(msg));
            }
        }
        catch (std::runtime_error& e) {
            BOOST_LOG_TRIVIAL(error) << "actor_system receive error: " << e.what();
        }
    }
    return;
}

std::shared_ptr<cancellable> actor_system::schedule_with_variant(std::unique_ptr<variant> variant, const actor_ref & target, const actor_ref & sender, long initial_delay_ms, long interval_ms) const
{
    return scheduler_->schedule(typed_message_factory::create(target, sender, std::move(variant)), initial_delay_ms, interval_ms);
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

void actor_system::add_actor(std::shared_ptr<abstract_actor> actor) {
    if (stopped_.load()) throw new actor_system_stopped(system_key_.to_string());

    std::lock_guard<std::mutex> guard(actors_write_mutex_);
    auto search = actors_.find(actor->actor_key());
    if (search != actors_.end()) {
        throw new actor_already_exists(actor->actor_key().to_string());
    }

    actors_.emplace(actor->actor_key(), std::move(actor));
}

}
