#pragma once

#include <vector>
#include "abstract_actor.h"
#include "../actor_system/actor_system_errors.h"

namespace bumbler {

class actor_system;
class untyped_actor;

class local_actor : public abstract_actor {
public:
    local_actor(const std::shared_ptr<actor_system>& actor_system, const std::string& name) :
            abstract_actor(actor_system, name),
            dispatcher_fun_([this] (const MessageVec& message_vec) -> int {
                for (int i = 0; i < message_vec.size(); i++) {
                    this->run_task(message_vec[i]->get_sender(), message_vec[i]->get_data());
                }
                return 0;
            })
    { }

    virtual ~local_actor();

    void init(std::unique_ptr<untyped_actor> u_actor) override;
    void stop_actor(bool wait = false) override;

    void create_internal_queue_thread();

    void read_messages();
    void add_message(std::unique_ptr<message> msg);

    void tell(std::unique_ptr<message> msg) override;

private:
    using MessageVec = std::vector<std::unique_ptr<message>>;

    std::mutex actor_thread_mutex_;
    std::future<void> queue_thread_future_;
    std::function<int(const MessageVec&)> dispatcher_fun_;
    std::condition_variable cv_;

    size_t calculate_throughput();
    void run_task(const actor_ref& sender, const boost::any& data);
};

}
