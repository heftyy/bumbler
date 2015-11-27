#pragma once

#include <iostream>
#include <atomic>
#include <boost/format.hpp>
#include <atan/actor_system/actor_system.h>
#include <communication/message_settings.h>

class out_router : public untyped_actor
{
public:
    out_router() {}

    ~out_router() {
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] destructor";
    }

protected:

    void on_receive(boost::any data) {
        messages_++;

        BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] on_receive thread id = " << std::this_thread::get_id();
        BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] received message from " << get_sender().actor_name << " message count = " << messages_;

        if(is_type<int>(data)) {
            int in = cast_message<int>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] message was " << in;
            reply(boost::str(boost::format("SERVER SAYS BLAM (%1%)") % in));
        }

        if(is_type<float>(data)) {
            float in = cast_message<float>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] message was " << in;
        }

        if(is_type<std::string>(data)) {
            std::string in = cast_message<std::string>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] message was " << in;
        }

        if(is_type<char*>(data)) {
            char* in = cast_message<char*>(data);
            BOOST_LOG_TRIVIAL(debug) << "[OUT_ROUTER] message was " << in;
        }

        //reply(msg_string, msg.sender_);
    }

private:
    int messages_;
};
