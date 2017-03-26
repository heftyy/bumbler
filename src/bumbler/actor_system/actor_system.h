#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include <boost/asio.hpp>
#include "../internal/bumbler.h"
#include "../dispatcher/dispatcher.h"
#include "../actor/actor_ref/actor_ref.h"

namespace bumbler {

class udp_server;
class actor_channel;
class abstract_actor;
class packet;
class cancellable;
class scheduler;

class actor_system : public std::enable_shared_from_this<actor_system> {
public:
    static std::shared_ptr<actor_system> create_system(const std::string& name, int port, int thread_pool_size = 5);

    actor_system(const std::string& name, int port);
    virtual ~actor_system();

    /*
     * if stop_mode is WAIT_FOR_QUEUE the dispatcher will wait for the all tasks to finish and actors in the system will
     * go through all of their messages
     *
     * if stop_mode is IGNORE_QUEUE system exits as soon as possible ignoring messages in the queue
     */
    void stop(stop_mode stop_mode = stop_mode::IGNORE_QUEUE);
    int stop_actor(const identifier& actor_name, stop_mode stop_mode = stop_mode::IGNORE_QUEUE);

    void tell_actor(std::unique_ptr<message> msg);
#if 0
    void ask_actor(std::unique_ptr<message> msg, const ResponseFun& response_fn);
#endif

    bool has_actor(const identifier& actor_ident);
    actor_ref get_actor_ref(const identifier& actor_name);
    std::unique_ptr<actor_channel> get_actor_channel(const identifier& actor_name);

    size_t actor_mailbox_size(const identifier& actor_ident);

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

    void add_actor(std::unique_ptr<abstract_actor> actor);

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

    std::future<int> dispatch(const DispatchFun& dispatch_fun, class local_actor& actor);

    void send_data(const std::string& data, const boost::asio::ip::udp::endpoint& target_endpoint);

    const identifier system_key() const {
        return system_key_;
    }

    bool started() {
        return started_;
    }

    bool stopped() {
        return stopped_;
    }

private:
    int port_;
    identifier system_key_;
    std::atomic<bool> started_;
    std::atomic<bool> stopped_;
    std::map<identifier, std::unique_ptr<abstract_actor>> actors_;
    std::mutex actors_write_mutex_;
    std::unique_ptr<udp_server> server_;
    std::unique_ptr<std::thread> io_service_thread_;
    boost::asio::io_service io_service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::unique_ptr<scheduler> scheduler_;
    std::unique_ptr<dispatcher> dispatcher_;

    void init(int thread_pool_size);

    void send_completed() { }

    /*
     * callback used by the udp server when a message is received
     */
    void receive(std::unique_ptr<packet> packet, const boost::asio::ip::udp::endpoint& sender_endpoint) const;

    std::shared_ptr<cancellable> schedule_with_variant(std::unique_ptr<variant> variant, const actor_ref& target, const actor_ref& sender, long initial_delay_ms, long interval_ms) const;
};

}
