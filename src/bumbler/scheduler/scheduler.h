#pragma once

#include <memory>

namespace bumbler {

class actor_system;
class cancellable;
class message;

class scheduler {
public:
    scheduler(std::shared_ptr<actor_system> system) : system_(system) {}
    std::shared_ptr<cancellable> schedule(std::unique_ptr<message> msg, long initial_delay_ms, long interval_ms) const;

private:
    std::weak_ptr<actor_system> system_;

    void send_message(std::unique_ptr<message> msg) const;
};

}
