#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
#include "udp_server.h"
#include "actor_system_errors.h"
#include "actor_system_storage.h"
#include "../messages/typed_message.h"
#include "../actor/actor.h"
#include "../actor/promise_actor.h"
#include "../packet/packet.h"
#include "../scheduler/scheduler.h"
#include "../scheduler/cancellable.h"

class actor_system : public std::enable_shared_from_this<actor_system> {
public:
    friend class actor;

    actor_system(const std::string& name, int port, int thread_pool_size = 5) : system_name_(name), port_(port) {
        scheduler_ = std::make_shared<scheduler>(thread_pool_size);
    }

    ~actor_system() {
        stop();
    }

    void init() {
        actor_system_storage::instance().add_system(shared_from_this());

        stopped_.store(false);
        boost::asio::io_service::work work(io_service_);

        server_ = std::shared_ptr<udp_server>(new udp_server(io_service_, port_));
        server_->send_completed_function = [&]() { send_completed(); };
        server_->receive_function = [&](std::unique_ptr<packet> packet,
                                        boost::asio::ip::udp::endpoint sender_endpoint) {
            receive(std::move(packet), sender_endpoint);
        };

        io_service_thread_ = std::unique_ptr<std::thread>(new std::thread(
                [this]() {
                    BOOST_LOG_TRIVIAL(debug) << "[ACTOR_SYSTEM_SERVER] STARTED ON PORT " << port_;
                    io_service_.run();
                }
        ));
    }

    /**
     * stop the actor system, if wait is true system is going to wait for actors to finish all their tasks in the queue
     */
    void stop(bool wait = false) {
        if (stopped_.load()) return;

        for (auto iter = actors_.begin(); iter != actors_.end();) {
            iter->second->stop_actor(wait);
            actors_.erase(iter++);
        }

        BOOST_LOG_TRIVIAL(debug) << "[ACTOR_SYSTEM] SHUTTING DOWN";
        stopped_.store(true);
        server_->stop();

        if (!io_service_.stopped()) io_service_.stop();
        if (io_service_thread_->joinable()) io_service_thread_->join();

        //workaround for windows
        //wait for the udp server to properly shutdown, otherwise it SIGSEGVs
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        server_.reset();

        actor_system_storage::instance().remove_system(system_name_);
    }

    template<class actor_type>
    int add_actor(std::shared_ptr<actor_type> actor) {
        if (stopped_) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);
        std::lock_guard<std::mutex> guard(actors_write_mutex_);
        auto search = actors_.find(actor->actor_name());
        if (search != actors_.end()) {
            return atan_error(ATAN_ACTOR_ALREADY_EXISTS, actor->actor_name());
        }

        actors_.emplace(actor->actor_name(), actor);
        return 0;
    }

    int stop_actor(std::string actor_name, bool wait = false) {
        if (stopped_) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);
        std::lock_guard<std::mutex> guard(actors_write_mutex_);

        auto search = actors_.find(actor_name);
        if (search != actors_.end()) {
            actors_[actor_name]->stop_actor(wait);
            actors_.erase(search);
        }
        else {
            return atan_error(ATAN_ACTOR_NOT_FOUND, actor_name);
        }
    }

    int tell_actor(std::unique_ptr<message> msg, bool from_remote = false) {
        if (stopped_.load()) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);

        if (msg->get_target().system_name.compare(system_name_) != 0)
            return atan_error(ATAN_WRONG_ACTOR_SYSTEM, msg->get_target().system_name);

        std::lock_guard<std::mutex> guard(actors_read_mutex_);
        std::string actor_name = msg->get_target().actor_name;

        auto search = actors_.find(actor_name);
        if (search != actors_.end()) {
            actors_[actor_name]->pass_message(std::move(msg), from_remote);
            return 0;
        }
        else {
            return atan_error(ATAN_ACTOR_NOT_FOUND, msg->get_target().actor_name);
        }
        //throw new actor_not_found(message.target.actor_name);
    }

    int future_tell_actor(std::unique_ptr<message> msg, std::function<void(boost::any)>& response_fn) {
        if (stopped_.load()) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);

        if (msg->get_target().system_name.compare(system_name_) != 0)
            return atan_error(ATAN_WRONG_ACTOR_SYSTEM, msg->get_target().system_name);

        std::lock_guard<std::mutex> guard(actors_read_mutex_);

        std::string actor_name = msg->get_target().actor_name;

        actor_ref p_actor = actor::create_actor<promise_actor>(get_next_temporary_actor_name(), shared_from_this(), response_fn);
        msg->set_sender(p_actor);

        auto search = actors_.find(actor_name);
        if (search != actors_.end()) {
            actors_[actor_name]->pass_message(std::move(msg), false);
            return 0;
        }
        else {
            return atan_error(ATAN_ACTOR_NOT_FOUND, msg->get_target().actor_name);
        }
        //throw new actor_not_found(message.target.actor_name);
    }

    actor_ref get_actor(std::string actor_name) {
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

    template<typename T>
    std::shared_ptr<cancellable> schedule(T& data, actor_ref target, long initial_delay_ms, long interval_ms) {
        this->schedule(data, target, actor_ref::none(), initial_delay_ms, interval_ms);
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule(T data, actor_ref target, actor_ref sender, long initial_delay_ms, long interval_ms) {
        typed_message<T> tm;
        tm.set_target(target);
        tm.set_sender(sender);
        tm.data = data;
        return scheduler_->schedule(tm, initial_delay_ms, interval_ms);
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule(typed_message<T>& msg, long initial_delay_ms, long interval_ms) {
        return scheduler_->schedule(msg, initial_delay_ms, interval_ms);
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule_once(typed_message<T>& msg, long initial_delay_ms) {
        return scheduler_->schedule_once(msg, initial_delay_ms);
    }

    std::shared_ptr<udp_server> get_server() {
        return server_;
    }

    std::shared_ptr<scheduler> get_scheduler() {
        return scheduler_;
    }

    std::string system_name() {
        return system_name_;
    }

private:
    int port_;
    std::atomic<bool> stopped_;
    std::string system_name_;
    std::map<std::string, std::shared_ptr<actor>> actors_;
    std::mutex actors_write_mutex_;
    std::mutex actors_read_mutex_;
    std::shared_ptr<udp_server> server_;
    std::unique_ptr<std::thread> io_service_thread_;
    boost::asio::io_service io_service_;
    std::shared_ptr<scheduler> scheduler_;

    void send_completed() { }

    void receive(std::unique_ptr<packet> packet, boost::asio::ip::udp::endpoint& sender_endpoint) {
//        BOOST_LOG_TRIVIAL(debug) << "Received data: \n" << packet->data.data;
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
                tell_actor(std::move(msg), true);
            }
            catch (std::runtime_error& e) {
                BOOST_LOG_TRIVIAL(error) << "actor_system receive error: " << e.what();
            }
        }
        return;
    }

    std::string get_next_temporary_actor_name() const {
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
};
