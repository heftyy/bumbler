#pragma once

#include <iostream>
#include <atomic>
#include <bumbler/messages/commands/commands.h>
#include <communication/message_settings.h>
#include <bumbler/actor/untyped_actor.h>
#include "typed_data.h"

using namespace bumbler;

int COMPUTE_LIMIT = 10000;

int compute(int in = 37) {
    // A bit like generating a hashcode. Pretty arbitrary choice,
    // but simple code which would be hard for the compiler to
    // optimise away.
    int running_total = 23;
    for (int i=0; i < COMPUTE_LIMIT; i++)
    {
        running_total = 37 * running_total + i;
    }
    return running_total;
}

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
    void on_receive(const boost::any& data) override {
        ++message_count;

        if(is_type<int>(data)) {
            int in = cast_message<int>(data);
            x += compute(in);
        }
    }
};

std::atomic<int> benchmark_actor::message_count(0);
