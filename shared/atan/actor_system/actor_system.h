
#include <iostream>
#include <mutex>
#include <thread>
#include <memory>
#include <chrono>
#include <future>
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
    static std::shared_ptr<actor_system> create_system(const std::string& name, int port, int thread_pool_size = 5);

    ~actor_system() {
        stop();
    }

    void stop(bool wait = false);

    template<class actor_type>
    int add_actor(std::unique_ptr<actor_type> actor) {
        if (stopped_) return atan_error(ACTOR_SYSTEM_STOPPED, system_name_);
        std::lock_guard<std::mutex> guard(actors_write_mutex_);
        auto search = actors_.find(actor->actor_name());
        if (search != actors_.end()) {
            return atan_error(ATAN_ACTOR_ALREADY_EXISTS, actor->actor_name());
        }

        actors_.emplace(actor->actor_name(), std::move(actor));
        return 0;
    }

    int stop_actor(std::string actor_name, bool wait = false);

    int tell_actor(std::unique_ptr<message> msg, bool from_remote = false);

    int future_tell_actor(std::unique_ptr<message> msg, std::function<void(boost::any)>& response_fn);

    const actor_ref get_actor(std::string actor_name);

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

    const std::shared_ptr<udp_server> get_server() const {
        return server_;
    }

    const std::shared_ptr<scheduler> get_scheduler() const {
        return scheduler_;
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
    actor_system(const std::string& name, int port, int thread_pool_size = 5) : system_name_(name), port_(port) {
        scheduler_ = std::make_shared<scheduler>(thread_pool_size);
    }

private:
    int port_;
    std::atomic<bool> started_;
    std::atomic<bool> stopped_;
    std::string system_name_;
    std::map<std::string, std::unique_ptr<actor>> actors_;
    std::mutex actors_write_mutex_;
    std::mutex actors_read_mutex_;
    std::shared_ptr<udp_server> server_;
    std::unique_ptr<std::thread> io_service_thread_;
    boost::asio::io_service io_service_;
    std::shared_ptr<scheduler> scheduler_;

    void init();

    void send_completed() { }

    void receive(std::unique_ptr<packet> packet, boost::asio::ip::udp::endpoint& sender_endpoint);

    const std::string get_next_temporary_actor_name() const;
};

//workaround for std::make_shared and protected constructor
struct concrete_actor_system : public actor_system {
    concrete_actor_system(const std::string &name, int port, int thread_pool_size)
            : actor_system(name, port, thread_pool_size) { }
};