#pragma once

#include <memory>
#include <map>
#include <mutex>
#include "../internal/identifier.h"

namespace bumbler {

class actor_system;

class actor_system_storage {
public:
    static actor_system_storage& instance() {
        static actor_system_storage instance;
        return instance;
    }

    void destroy();

    void register_system(const std::shared_ptr<actor_system>& actor_system);
    void remove_system(const identifier& system_ident);

    std::shared_ptr<actor_system> get_system(const identifier& system_ident);
    std::shared_ptr<actor_system> any();
    bool has_system(const identifier& system_ident);

private:
    actor_system_storage() { }

    // C++ 11
    // =======
    // We can use the better technique of deleting the methods we don't want.
    actor_system_storage(actor_system_storage const&) = delete;

    void operator=(actor_system_storage const&) = delete;

    ~actor_system_storage();

    std::map<identifier, std::shared_ptr<actor_system>> systems_;
    std::mutex systems_mutex_;
};

}
