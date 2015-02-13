#include "actor_system_storage.h"
#include "actor_system.h"

void actor_system_storage::destroy()
{
    this->systems_.clear();
}

void actor_system_storage::add_system(std::shared_ptr<actor_system> actor_system)
{
    std::lock_guard<std::mutex> guard(this->systems_mutex_);
    systems_.push_back(actor_system);
}

std::shared_ptr<actor_system> actor_system_storage::get_system(std::string system_name)
{
    std::lock_guard<std::mutex> guard(this->systems_mutex_);
    for(std::shared_ptr<actor_system> system : systems_)
    {
        if(system_name.compare(system->system_name()) == 0)
        {
            return system;
        }
    }

    return nullptr;
}