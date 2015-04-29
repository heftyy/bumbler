#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
#include "udp_server.h"
#include "actor_system_errors.h"
#include "actor_system_storage.h"
#include "../message.h"
#include "../actor/actor.h"
#include "../packet/packet.h"
#include "../scheduler/scheduler.h"

class actor_system : public std::enable_shared_from_this<actor_system>
{
public:
    friend class actor;
    actor_system(const std::string& name, int port) : system_name_(name), port_(port) {}

    ~actor_system()
    {
        stop();
    }

    void init()
    {
        actor_system_storage::instance().add_system(shared_from_this());

        stopped_.store(false);
        boost::asio::io_service::work work(io_service_);

        server_ = std::shared_ptr<udp_server>(new udp_server(io_service_, port_));
        server_->send_completed_function = [&](){ send_completed(); };
        server_->receive_function = [&](std::unique_ptr<packet> packet, boost::asio::ip::udp::endpoint sender_endpoint) { receive(std::move(packet), sender_endpoint); };

        io_service_thread_ = std::unique_ptr<std::thread>(new std::thread([this]()
                                                                          {
                                                                              BOOST_LOG_TRIVIAL(debug) << "[ACTOR_SYSTEM] STARTED ON PORT " << port_;
                                                                              io_service_.run();
                                                                          }));

        scheduler_ = std::shared_ptr<scheduler>(new scheduler());
    }

    void stop()
    {
        if (stopped_.load()) return;

        for(auto iter = actors_.begin(); iter != actors_.end(); )
        {
            iter->second->stop_actor();
            actors_.erase(iter++);
        }

        BOOST_LOG_TRIVIAL(debug) << "[ACTOR_SYSTEM] SHUTTING DOWN";
        stopped_.store(true);
        server_->stop();

        if (!io_service_.stopped()) io_service_.stop();
        if (io_service_thread_->joinable()) io_service_thread_->join();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        server_.reset();
    }

    template<class actor_type>
    int add_actor(std::shared_ptr<actor_type> actor)
    {
        if (stopped_) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);
        std::lock_guard<std::mutex> guard(actors_mutex_);
        auto search = actors_.find(actor->actor_name());
        if(search != actors_.end())
        {
            return atan_error(ATAN_ACTOR_ALREADY_EXISTS, actor->actor_name());
        }

        actors_.emplace(actor->actor_name(), actor);
        return 0;
    }

    int remove_actor(std::shared_ptr<actor> actor)
    {
        if (stopped_) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);
        std::lock_guard<std::mutex> guard(actors_mutex_);

        auto search = actors_.find(actor->actor_name());
        if(search != actors_.end())
        {
            actors_.erase(search);
        }
        else
        {
            return atan_error(ATAN_ACTOR_NOT_FOUND, actor->actor_name());
        }
        //throw new actor_not_found(actor->actor_name());
    }

    int tell_actor(const message& message)
    {
        if (stopped_.load()) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);

        if (message.target.system_name.compare(system_name_) != 0) return atan_error(ATAN_WRONG_ACTOR_SYSTEM, system_name_);
        //if (message.target.system_name.compare(system_name_) != 0) throw new wrong_actor_system(system_name_);
        std::lock_guard<std::mutex> guard(actors_mutex_);

        std::string actor_name = message.target.actor_name;

        auto search = actors_.find(actor_name);
        if(search != actors_.end())
        {
            actors_[actor_name]->tell(message);
            return 0;
        }
        else
        {
            return atan_error(ATAN_ACTOR_NOT_FOUND, message.target.actor_name);
        }
        //throw new actor_not_found(message.target.actor_name);
    }

    actor_ref get_actor(std::string actor_name)
    {
        if (stopped_) nullptr;
        std::lock_guard<std::mutex> guard(actors_mutex_);

        auto search = actors_.find(actor_name);
        if(search != actors_.end())
        {
            return actors_[actor_name]->get_self();
        }
        else
        {
            return actor_ref::none();
        }
    }

    std::shared_ptr<udp_server> server()
    {
        return server_;
    }

    std::string system_name()
    {
        return system_name_;
    }

    void schedule(message& message, long initial_delay_ms, long interval_delay_ms)
    {
        scheduler_->schedule(message, initial_delay_ms, interval_delay_ms);
    }

    void schedule_once(message& message, long initial_delay_ms)
    {
        scheduler_->schedule_once(message, initial_delay_ms);
    }

private:
    int port_;
    std::atomic<bool> stopped_;
    std::string system_name_;
    std::map<std::string, std::shared_ptr<actor>> actors_;
    std::mutex actors_mutex_;
    std::shared_ptr<udp_server> server_;
    std::unique_ptr<std::thread> io_service_thread_;
    boost::asio::io_service io_service_;
    std::shared_ptr<scheduler> scheduler_;

    void send_completed()
    {
        //std::cout << "SEND COMPLETED" << std::endl;
    }

    void receive(std::unique_ptr<packet> packet, boost::asio::ip::udp::endpoint& sender_endpoint)
    {
        message msg;
        message::restore_message(msg, packet->data.data);
        msg.sender.ip = sender_endpoint.address().to_string();
        msg.sender.port = sender_endpoint.port();

        if (msg.target.exists())
        {
            try
            {
                tell_actor(msg);
            }
            catch (std::runtime_error& e)
            {
                BOOST_LOG_TRIVIAL(error) << "actor_system receive error: " << e.what();
            }
        }
        return;

    }
};
