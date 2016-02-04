#pragma once

#include <iostream>
#include <atomic>
#include <bumbler/messages/commands/commands.h>
#include <communication/message_settings.h>
#include <bumbler/actor/untyped_actor.h>
#include "typed_data.h"

using namespace bumbler;

class benchmark_actor : public untyped_actor {
public:
    static std::atomic<int> message_count;
    int x;

    benchmark_actor() {}
    benchmark_actor(int x) : x(x) {}

    ~benchmark_actor() {
        BOOST_LOG_TRIVIAL(debug) << "[BENCHMARK_ACTOR] destructor";
    }

protected:
    void on_receive(boost::any data) override {
	    ++message_count;

        if(is_type<int>(data)) {
            int in = cast_message<int>(data);
        }
    }
};

std::atomic<int> benchmark_actor::message_count(0);
