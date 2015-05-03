#pragma once

#include <iostream>
#include <atomic>
#include <atan/actor/local_actor.h>
#include <atan/actor_system/actor_system.h>
#include <communication/message_settings.h>

class out_actor : public local_actor {
public:
    out_actor(std::string name, std::shared_ptr<actor_system> actor_system)
            : local_actor(name, actor_system) { }

    ~out_actor() {
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] destructor";
    }

private:

    template<typename T>
    void on_receive(T data) override {
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] received message from " << get_sender().actor_name;
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << data;

        //reply(msg_string, msg.sender);
    }
};

