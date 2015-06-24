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

protected:

    void on_receive(boost::any data) {
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] received message from " << get_sender().actor_name;

        if(is_type<int>(data)) {
            int in = cast_message<int>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << in;
        }

        if(is_type<float>(data)) {
            float in = cast_message<float>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << in;
            reply("1112321321");
        }

        if(is_type<std::string>(data)) {
            std::string in = cast_message<std::string>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << in;
        }

        if(is_type<char*>(data)) {
            char* in = cast_message<char*>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << in;
        }

        //reply(msg_string, msg.sender_);
    }
};
