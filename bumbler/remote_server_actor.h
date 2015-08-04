#pragma once

#include <iostream>
#include <atomic>
#include <atan/actor/remote_actor.h>
#include <communication/message_settings.h>

class remote_server_actor : public remote_actor {
public:
    remote_server_actor(std::string name, std::string remote_actor_ref, std::shared_ptr<actor_system> actor_system)
            : remote_actor(name, actor_system, actor_ref(remote_actor_ref)) { }

    ~remote_server_actor() {
        BOOST_LOG_TRIVIAL(debug) << "[REMOTE_ACTOR] destructor";
    }

protected:

    void on_receive(boost::any data) {
        std::string in = cast_message<std::string>(data);

        BOOST_LOG_TRIVIAL(debug) << "[REMOTE_ACTOR] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[REMOTE_ACTOR] received message from " << get_sender().actor_name;
        BOOST_LOG_TRIVIAL(debug) << "[REMOTE_ACTOR] message was " << in;

        //reply(msg_string, msg.sender);
    }
};

