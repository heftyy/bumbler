#pragma once

#include <iostream>
#include <atomic>
#include <bumbler/messages/commands/commands.h>
#include <communication/message_settings.h>
#include <bumbler/actor/untyped_actor.h>

class server_actor : public untyped_actor {
protected:
    void on_receive(const boost::any& data) override {

        BOOST_LOG_TRIVIAL(debug) << "[server_actor] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "server_actor received message from " << get_sender().actor_key.to_string();

        if(is_type<std::string>(data)) {
            BOOST_LOG_TRIVIAL(debug) << "message was " << cast_message<std::string>(data);
        }
        else if(is_type<int>(data)) {
            BOOST_LOG_TRIVIAL(debug) << "message was " << cast_message<int>(data);
        }
        else if(is_type<float>(data)) {
            BOOST_LOG_TRIVIAL(debug) << "message was " << cast_message<float>(data);
            reply("pi");
        }

    }
};

