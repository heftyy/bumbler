#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include <boost/asio.hpp>
#include "../internal/bumbler.h"
#include "../actor/actor_ref/actor_ref.h"

namespace bumbler {

class dispatcher;
class udp_server;
class actor_channel;
class abstract_actor;
class packet;
class cancellable;
class message;
class scheduler;

class actor_system : public std::enable_shared_from_this<actor_system> {
public:
    static std::shared_ptr<actor_system> create_system(const std::string& name, int port, int thread_pool_size = 5);

    virtual ~actor_system() {
        stop();
    }

    /*
     * if stop_mode is WAIT_FOR_QUEUE the dispatcher will wait for the all tasks to finish and actors in the system will
     * go through all of their messages
     *
     * if stop_mode is IGNORE_QUEUE system exits as soon as possible ignoring messages in the queue
     */
    void stop(stop_mode stop_mode = stop_mode::IGNORE_QUEUE);

    int stop_actor(const std::string& actor_name, stop_mode stop_mode = stop_mode::IGNORE_QUEUE);

#if 0
    int tell_actor(std::unique_ptr<message> msg);
    int ask_actor(std::unique_ptr<message> msg, const ResponseFun& response_fn);
#endif

    actor_ref get_actor_ref(const std::string& actor_name);
    std::unique_ptr<actor_channel> get_actor_channel(const std::string& actor_name);

    /*
     * generate a new name for a temporary actor
     */
    std::string get_next_temporary_actor_name() const;

    template<typename Props, typename ...ActorArgs>
    actor_ref actor_of(Props&& props, ActorArgs&&... actor_args) {
        auto actor_ptr = props.create_actor_instance(shared_from_this(), std::forward<ActorArgs>(actor_args)...);
        auto ref = actor_ptr->get_self();
        add_actor(std::move(actor_ptr));

        return ref;
    }

    int add_actor(std::shared_ptr<abstract_actor> actor);

	template<typename T>
	std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, long initial_delay_ms, long interval_ms = 0) const {
		return this->schedule(std::forward<T>(data), target, actor_ref::none(), initial_delay_ms, interval_ms);
	}

	template<typename T>
	std::shared_ptr<cancellable> schedule(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms, long interval_ms = 0) const {
		auto variant = typed_variant_factory::create(std::forward<T>(data));
		return schedule_with_variant(std::move(variant), target, sender, initial_delay_ms, interval_ms);
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, long initial_delay_ms = 0) const {
        return this->schedule(std::forward<T>(data), target, actor_ref::none(), initial_delay_ms, 0);
    }

    template<typename T>
    std::shared_ptr<cancellable> schedule_once(T&& data, const actor_ref& target, const actor_ref& sender, long initial_delay_ms = 0) const {
		auto variant = typed_variant_factory::create(std::forward<T>(data));
		return schedule_with_variant(std::move(variant), target, sender, initial_delay_ms, 0);
    }

    std::shared_ptr<udp_server> get_server() const {
        return server_;
    }

    std::shared_ptr<scheduler> get_scheduler() const {
        return scheduler_;
    }

    std::shared_ptr<dispatcher> get_dispatcher() const {
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
    actor_system(const std::string& name, int port, int thread_pool_size = 5);

private:
    int port_;
    std::atomic<bool> started_;
    std::atomic<bool> stopped_;
    std::string system_name_;
    std::map<std::string, std::shared_ptr<abstract_actor>> actors_;
    std::mutex actors_write_mutex_;
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
    void receive(std::unique_ptr<packet> packet, const boost::asio::ip::udp::endpoint& sender_endpoint) const;

	std::shared_ptr<cancellable> schedule_with_variant(std::unique_ptr<variant> variant, const actor_ref& target, const actor_ref& sender, long initial_delay_ms, long interval_ms) const;
};

//workaround for std::make_shared and protected constructor
struct concrete_actor_system : public actor_system {
    concrete_actor_system(const std::string &name, int port, int thread_pool_size)
            : actor_system(name, port, thread_pool_size) { }
};

}
