#include "actor_system_storage.h"
#include "actor_system.h"

namespace bumbler {

actor_system_storage::~actor_system_storage() {
    this->destroy();
}

void actor_system_storage::destroy() {
    this->systems_.clear();
}

void actor_system_storage::add_system(std::shared_ptr<actor_system> actor_system) {
    std::lock_guard<std::mutex> guard(this->systems_mutex_);
    systems_.insert(std::make_pair(actor_system->system_name(), actor_system));
}

void actor_system_storage::remove_system(std::string system_name) {
    std::lock_guard<std::mutex> guard(this->systems_mutex_);
    auto search = systems_.find(system_name);
    if (search != systems_.end()) {
        systems_.erase(search);
    }
}

std::shared_ptr<actor_system> actor_system_storage::get_system(std::string system_name) {
    std::lock_guard<std::mutex> guard(this->systems_mutex_);
    for (auto& pair : systems_) {
        if (system_name.compare(pair.second->system_name()) == 0) {
            return pair.second;
        }
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

}
