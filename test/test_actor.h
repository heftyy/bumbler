#pragma once

#include <iostream>
#include <atomic>
#include <bumbler/messages/commands/commands.h>
#include <communication/message_settings.h>
#include <bumbler/actor/untyped_actor.h>
#include "typed_data.h"

using namespace bumbler;

class test_actor : public untyped_actor {
public:
    static std::atomic<int> message_count;
    int x;

    test_actor() {}
    test_actor(int x) : x(x) {}

    ~test_actor() {
        BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] destructor";
    }

protected:
    void on_receive(const boost::any& data) override {
        ++message_count;

        BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] received message from " << get_sender().actor_key.to_string();

        if(is_type<int>(data)) {
            int in = cast_message<int>(data);
            BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] message was " << in;
            reply("BLAM");
        }

        if(is_type<float>(data)) {
            float in = cast_message<float>(data);
            BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] message was " << in;
        }

        if(is_type<std::string>(data)) {
            std::string in = cast_message<std::string>(data);
            BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] message was " << in;
            reply(::stop_actor("HAH!"));
        }

        if(is_type<char*>(data)) {
            char* in = cast_message<char*>(data);
            BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] message was " << in;
        }

        if(is_type<typed_data<int>>(data)) {
            reply("BLAM");
        }

        if(is_type<typed_data<std::string>>(data)) {
            typed_data<std::string> in = cast_message<typed_data<std::string>>(data);
            BOOST_LOG_TRIVIAL(debug) << "[TEST_ACTOR] message was typed_data " << in.data;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        //reply(msg_string, msg.sender_);
    }
};

std::atomic<int> test_actor::message_count(0);
