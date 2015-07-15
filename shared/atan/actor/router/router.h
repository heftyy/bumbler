#pragma once

#include <list>
#include <memory>
#include "../actor.h"
#include "../../thread_pool/ctpl_stl.h"

class router : public actor
{
protected:
    int size;
    ctpl::thread_pool thread_pool;
    std::vector<std::unique_ptr<actor>> actors;

    router(const std::string& name, std::shared_ptr<actor_system>& actor_system, int size)
            : actor(name, actor_system), size(size) {
    }

    actor_ref init();
    virtual void tell_actor(std::unique_ptr<message> msg) = 0;
    virtual void tell(std::unique_ptr<message> msg, bool remote = false);
    void tell_all(std::unique_ptr<message> msg);

    template<typename T>
    void create_actors() {
        for(int i = 0; i < size; i++) {
            this->actors.emplace_back(
                    std::unique_ptr<T>(new T(this->actor_name_, this->actor_system_.lock()))
            );
        }
    }

    actor& get_actor(int i) {
        return *this->actors[i];
    }

private:

    void stop_actor(bool wait = false) {
        actor::stop_actor(wait);
        this->thread_pool.stop(wait);
    }

};

