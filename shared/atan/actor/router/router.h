#pragma once

#include <list>
#include <memory>
#include <functional>
#include "../actor.h"
#include "../../thread_pool/ctpl_stl.h"

class router : public actor
{
public:
    void stop_actor(bool wait = false) override {
        actor::stop_actor(wait);
        this->thread_pool.stop(wait);
    }

protected:
    int size;
    ctpl::thread_pool thread_pool;
    std::vector<std::unique_ptr<actor>> actors;

    router(const std::string& name, const std::shared_ptr<actor_system>& actor_system, int size)
            : actor(name, actor_system), size(size) {
    }

    ~router() {
        this->thread_pool.stop();
    }

    actor_ref init() override;
    virtual void tell(std::unique_ptr<message> msg, bool remote = false) override;
    virtual void tell_one(std::unique_ptr<message> msg) = 0;
    void tell_all(std::unique_ptr<message> msg);

    template<typename T>
    void create_actors() {
        for(int i = 0; i < size; i++) {
            std::unique_ptr<T> a = std::unique_ptr<T>(new T(this->actor_name_, this->actor_system_.lock()));
            this->actors.push_back(std::move(a));
        }
    }

    actor& get_actor(int i) {
        return *this->actors[i];
    }


};

