#define BOOST_TEST_MODULE MESSAGES_BENCHMARK

#include <memory>
#include <future>
#include <chrono>
#include <boost/test/unit_test.hpp>
#include <bumbler/thread_pool/thread_pool.h>
#include <bumbler/actor_system/actor_system.h>
#include <bumbler/actor/local_actor.h>
#include <bumbler/logger/logger.h>
#include <bumbler/actor/routing/round_robin_pool.h>
#include "benchmark_actor.h"

using namespace bumbler;

//int MESSAGES_TO_SEND = 5 * 1000 * 1000;
int MESSAGES_TO_SEND = 500 * 1000;

BOOST_AUTO_TEST_SUITE(messages_benchmark)

BOOST_AUTO_TEST_CASE(MessagesActorBenchmark) {
    boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= boost::log::trivial::info);
    benchmark_actor::message_count = 0;

    auto system1 = actor_system::create_system("test_system1", 4555);

    auto props_local = typed_props<local_actor, benchmark_actor>();
    auto la1 = system1->actor_of(props_local, "benchmark_actor");

    auto start = std::chrono::steady_clock::now();

    for(int i = 0; i < MESSAGES_TO_SEND; i++) {
        la1.tell(7);
    }

    system1->stop(true);

    auto end = std::chrono::steady_clock::now();

    std::cout << "local_actor " << std::chrono::duration <double, std::milli> (end - start).count() << " ms" << std::endl;

    BOOST_CHECK_EQUAL(MESSAGES_TO_SEND, benchmark_actor::message_count.load());
}


BOOST_AUTO_TEST_CASE(MessagesRouterBenchmark) {
    boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= boost::log::trivial::info);
    benchmark_actor::message_count = 0;

    auto system1 = actor_system::create_system("test_system1", 4555, 10);

    auto props_router = typed_props<router, benchmark_actor>();
    props_router.with_router<round_robin_pool>(10);

    auto r1 = system1->actor_of(props_router, "benchmark_actor");

    auto start = std::chrono::steady_clock::now();

    for(int i = 0; i < MESSAGES_TO_SEND; i++) {
        r1.tell(7);
    }

    system1->stop(true);

    auto end = std::chrono::steady_clock::now();

    std::cout << "router(5)   " << std::chrono::duration <double, std::milli> (end - start).count() << " ms" << std::endl;

    BOOST_CHECK_EQUAL(MESSAGES_TO_SEND, benchmark_actor::message_count.load());
}

BOOST_AUTO_TEST_SUITE_END()