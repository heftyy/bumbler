#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
#include <chrono>
#include <future>
#include "udp_server.h"
#include "actor_system_errors.h"
#include "actor_system_storage.h"
#include "typed_promise_actor.h"
#include "../messages/typed_message.h"
#include "../actor/abstract_actor.h"
#include "../actor/promise_actor.h"
#include "../packet/packet.h"
#include "../scheduler/scheduler.h"
#include "../scheduler/cancellable.h"
#include "../dispatcher/dispatcher.h"
#include "../actor/props/typed_props.h"

namespace bumbler {

class actor_system : public std::enable_shared_from_this<actor_system> {
public:
    static std::shared_ptr<actor_system> create_system(const std::string& name, int port, int thread_pool_size = 5);

    virtual ~actor_system() {
        stop();
    }

    /*
     * if wait is true the dispatcher will wait for the all tasks to finish and actors in the system will
     * go through all of their messages
     *
     * if wait is false system exits as soon as possible ignoring messages in the queue
     */
    void stop(bool wait = false);

    int stop_actor(std::string actor_name, bool wait = false);

    int tell_actor(std::unique_ptr<message> msg, bool from_remote = false);
    int ask_actor(std::unique_ptr<message> msg, const std::function<void(boost::any)>& response_fn);

    const actor_ref get_actor(std::string actor_name);

    template<typename Props, typename ...ActorArgs>
    actor_ref actor_of(Props&& props, ActorArgs&&... actor_args) {
        std::unique_ptr<abstract_actor> actor_ptr = props.create_actor_instance(shared_from_this(), std::forward<ActorArgs>(actor_args)...);
        actor_ref ref = actor_ptr->get_self();
        add_actor(std::move(actor_ptr));

        return ref;
    }

    int add_actor(std::unique_ptr<abstract_actor> actor);

	template<typename T>
	std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, long initial_delay_ms, long interval_ms = 0) const {
		return this->schedule(std::forward<T>(data), target, actor_ref::none(), initial_delay_ms, interval_ms);
	}

	template<typename T>
	std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms, long interval_ms = 0) const {
        return scheduler_->schedule(std::move(typed_message_factory::create(target, sender, std::forward<T>(data))), initial_delay_ms, interval_ms);
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, long initial_delay_ms = 0) const {
        return this->schedule(std::forward<T>(data), target, actor_ref::none(), initial_delay_ms, 0);
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms = 0) const {
        return scheduler_->schedule(std::move(typed_message_factory::create(target, sender, std::forward<T>(data))), initial_delay_ms, 0);
    }

    const std::shared_ptr<udp_server> get_server() const {
        return server_;
    }

    const std::shared_ptr<scheduler> get_scheduler() const {
        return scheduler_;
    }

    const std::shared_ptr<dispatcher> get_dispatcher() const {
        return dispatcher_;
    }

    const std::string system_name() const {
        return system_name_;
    }

    bool started() {
        return started_;
    }

    bool stopped() {
        return stopped_;
    }

protected:
    actor_system(const std::string& name, int port, int thread_pool_size = 5) : port_(port), system_name_(name) {
        dispatcher_ = std::make_shared<dispatcher>(thread_pool_size);
        scheduler_ = std::make_shared<scheduler>(dispatcher_);
    }

private:
    int port_;
    std::atomic<bool> started_;
    std::atomic<bool> stopped_;
    std::string system_name_;
    std::map<std::string, std::unique_ptr<abstract_actor>> actors_;
    std::mutex actors_write_mutex_;
    std::mutex actors_read_mutex_;
    std::shared_ptr<udp_server> server_;
    std::unique_ptr<std::thread> io_service_thread_;
    boost::asio::io_service io_service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::shared_ptr<scheduler> scheduler_;
    std::shared_ptr<dispatcher> dispatcher_;

    void init();

    void send_completed() { }

    /*
     * callback used by the udp server when a message is received
     */
    void receive(std::unique_ptr<packet> packet, boost::asio::ip::udp::endpoint& sender_endpoint);


    /*
     * generate a new name for a temporary actort
     */
    const std::string get_next_temporary_actor_name() const;
};

//workaround for std::make_shared and protected constructor
struct concrete_actor_system : public actor_system {
    concrete_actor_system(const std::string &name, int port, int thread_pool_size)
            : actor_system(name, port, thread_pool_size) { }
};

}
