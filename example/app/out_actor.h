#pragma once

#include <iostream>
#include <atomic>
#include <bumbler/actor/local_actor.h>
#include <bumbler/messages/commands/commands.h>
#include <communication/message_settings.h>

class out_actor : public bumbler::untyped_actor {
public:
    out_actor() {}

    ~out_actor() {
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] destructor";
    }

protected:
    void on_receive(const boost::any& data) {
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] received message from " << get_sender().actor_name;

        if(is_type<int>(data)) {
            int in = cast_message<int>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << in;
            reply("BLAM");
        }

        if(is_type<float>(data)) {
            float in = cast_message<float>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << in;
        }

        if(is_type<std::string>(data)) {
            std::string in = cast_message<std::string>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << in;
            reply(::stop_actor<std::string>("HAH!"));
        }

        if(is_type<char*>(data)) {
            char* in = cast_message<char*>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ACTOR] message was " << in;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        //reply(msg_string, msg.sender_);
    }
};
