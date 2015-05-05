#pragma once

#include <iostream>
#include <atomic>
#include <atan/interruptible_thread.h>
#include <atan/actor/local_actor.h>
#include <atan/actor_system/actor_system.h>

class server_actor : public local_actor {
public:

    server_actor(std::string name, std::shared_ptr<actor_system> actor_system)
            : local_actor(name, actor_system) {
    }

    ~server_actor() {
    }

private:

    void on_receive(boost::any data) {

        std::string msg = cast_message<std::string>(data);

        BOOST_LOG_TRIVIAL(debug) << "[server_actor] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "server_actor received message from " << get_sender().actor_name;
        BOOST_LOG_TRIVIAL(debug) << "message was " << msg;
    }
};

