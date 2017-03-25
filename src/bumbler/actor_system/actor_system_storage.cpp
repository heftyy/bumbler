#include "actor_system_storage.h"
#include "actor_system.h"
#include "../messages/message.h"

namespace bumbler {

actor_system_storage::~actor_system_storage() {
    this->destroy();
}

void actor_system_storage::destroy() {
    this->systems_.clear();
}

void actor_system_storage::register_system(const std::shared_ptr<actor_system>& actor_system) {
    std::lock_guard<std::mutex> guard(this->systems_mutex_);
    systems_.insert(std::make_pair(actor_system->system_key(), actor_system));
}

void actor_system_storage::remove_system(const identifier& system_ident) {
    std::lock_guard<std::mutex> guard(this->systems_mutex_);
    auto search = systems_.find(system_ident);
    if (search != systems_.end()) {
        systems_.erase(search);
    }
}

std::shared_ptr<actor_system> actor_system_storage::get_system(const identifier& system_ident) {
    std::lock_guard<std::mutex> guard(this->systems_mutex_);
    auto search = systems_.find(system_ident);
    if (search != systems_.end()) {
        return systems_[system_ident];
    }

    return nullptr;
}

std::shared_ptr<actor_system> actor_system_storage::any() {
    std::lock_guard<std::mutex> guard(this->systems_mutex_);

    if(systems_.size() > 0) {
        return systems_.begin()->second;
    }

    return nullptr;
}

bool actor_system_storage::has_system(const identifier& system_ident)
{
    auto search = systems_.find(system_ident);
    return search != systems_.end();
}

}
