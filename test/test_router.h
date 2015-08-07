#pragma once

#include <iostream>
#include <atomic>
#include <atan/messages/commands/commands.h>
#include <communication/message_settings.h>
#include <atan/actor/untyped_actor.h>

class test_router : public untyped_actor {
public:
    static std::atomic<int> message_count;

    test_router() {}

    ~test_router() {
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] destructor";
    }

protected:
    void on_receive(boost::any data) override {
	    ++message_count;

        BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] received message from " << get_sender().actor_name;

        if(is_type<int>(data)) {
            int in = cast_message<int>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] message was " << in;
            reply("BLAM");
        }

        if(is_type<float>(data)) {
            float in = cast_message<float>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] message was " << in;
        }

        if(is_type<std::string>(data)) {
            std::string in = cast_message<std::string>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] message was " << in;
            reply(::stop_actor<std::string>("HAH!"));
        }

        if(is_type<char*>(data)) {
            char* in = cast_message<char*>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] message was " << in;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        //reply(msg_string, msg.sender_);
    }
};

std::atomic<int> test_router::message_count(0);