#define BOOST_TEST_MODULE MESSAGES_BENCHMARK

#include <memory>
#include <future>
#include <boost/test/unit_test.hpp>
#include <bumbler/actor/props/typed_props.h>
#include <bumbler/thread_pool/thread_pool.h>
#include <bumbler/actor_system/actor_system.h>
#include <bumbler/actor/routing/round_robin_pool.h>
#include "benchmark_actor.h"

using namespace bumbler;

//int MESSAGES_TO_SEND = 5 * 1000 * 1000;
int MESSAGES_TO_SEND = 500 * 1000;

BOOST_AUTO_TEST_SUITE(messages_benchmark)

BOOST_AUTO_TEST_CASE(LoopBenchmark) {
    benchmark_actor::message_count = 0;

    auto start = std::chrono::steady_clock::now();

    int x = 5;

    for (int i = 0; i < MESSAGES_TO_SEND; i++) {
        benchmark_actor::message_count++;
        x += compute();
    }

    auto end = std::chrono::steady_clock::now();

    std::cout << "loop        " << std::chrono::duration<double, std::milli>(end - start).count() << " ms, " << MESSAGES_TO_SEND << " messages " << x << std::endl;

    BOOST_CHECK_EQUAL(MESSAGES_TO_SEND, benchmark_actor::message_count.load());
}

BOOST_AUTO_TEST_CASE(MessagesActorBenchmark) {
    boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= boost::log::trivial::info);
    benchmark_actor::message_count = 0;

    auto system1 = actor_system::create_system("test_system1", 4555);

    auto props_local = typed_props<local_actor, benchmark_actor>();
    auto la1 = system1->actor_of(props_local, "benchmark_actor");

    auto start = std::chrono::steady_clock::now();

    for(int i = 0; i < MESSAGES_TO_SEND; i++) {
        la1.tell(37);
    }

    system1->stop(true);

    auto end = std::chrono::steady_clock::now();

    std::cout << "local_actor " << std::chrono::duration<double, std::milli>(end - start).count() << " ms, " << MESSAGES_TO_SEND << " messages" << std::endl;

    BOOST_CHECK_EQUAL(MESSAGES_TO_SEND, benchmark_actor::message_count.load());
}

BOOST_AUTO_TEST_CASE(MessagesRouterBenchmark) {
    boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= boost::log::trivial::info);
    benchmark_actor::message_count = 0;
    int router_size = 5;

    auto system1 = actor_system::create_system("test_system1", 4555, router_size);

    auto props_router = typed_props<router, benchmark_actor>();
    props_router.with_router<round_robin_pool>(router_size);

    auto r1 = system1->actor_of(props_router, "benchmark_actor");

    auto start = std::chrono::steady_clock::now();

    for(int i = 0; i < MESSAGES_TO_SEND; i++) {
        r1.tell(37);
    }

    system1->stop(true);

    auto end = std::chrono::steady_clock::now();

    std::cout << "router(" << router_size << ")   " << std::chrono::duration<double, std::milli>(end - start).count() << " ms, " << MESSAGES_TO_SEND << " messages" << std::endl;

    BOOST_CHECK_EQUAL(MESSAGES_TO_SEND, benchmark_actor::message_count.load());
}

BOOST_AUTO_TEST_SUITE_END()
