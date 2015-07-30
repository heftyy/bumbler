#pragma once

#include <memory>
#include <map>
#include <iostream>
#include <mutex>

class actor_system;

class actor_system_storage {
public:
    static actor_system_storage& instance() {
        static actor_system_storage instance;
        return instance;
    }

    void destroy();

    void add_system(std::shared_ptr<actor_system> actor_system);

    void remove_system(std::string system_name);

    std::shared_ptr<actor_system> get_system(std::string system_name);

private:
    actor_system_storage() { }

    // C++ 11
    // =======
    // We can use the better technique of deleting the methods we don't want.
    actor_system_storage(actor_system_storage const&) = delete;

    void operator=(actor_system_storage const&) = delete;

    ~actor_system_storage();

    std::map<std::string, std::shared_ptr<actor_system>> systems_;
    std::mutex systems_mutex_;
};