#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <mutex>

class actor_system;

class actor_system_storage
{
public:
    static actor_system_storage& instance()
    {
        static actor_system_storage instance;
        return instance;
    }

    void add_system(std::shared_ptr<actor_system> actor_system);

    std::shared_ptr<actor_system> get_system(std::string system_name);

private:
    actor_system_storage() {}
    actor_system_storage(actor_system_storage const&);              // Don't Implement.
    void operator=(actor_system_storage const&); // Don't implement

    std::vector<std::shared_ptr<actor_system>> systems_;
    std::mutex systems_mutex_;
};