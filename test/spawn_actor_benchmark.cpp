#define BOOST_TEST_MODULE SPAWN_ACTOR_BENCHMARK

#include <memory>
#include <future>
#include <boost/test/unit_test.hpp>
#include <bumbler/thread_pool/thread_pool.h>
#include <bumbler/actor_system/actor_system.h>
#include <bumbler/actor/local_actor.h>
#include "benchmark_actor.h"

using namespace bumbler;

int ACTORS_TO_CREATE = 1000;

BOOST_AUTO_TEST_SUITE(actor_benchmark)


BOOST_AUTO_TEST_CASE(SpawnActorBenchmark) {
    boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= boost::log::trivial::info);

    auto system1 = actor_system::create_system("test_system1", 4555);

    auto props_local = typed_props<local_actor, benchmark_actor>();

    for(int i = 0; i < ACTORS_TO_CREATE; i++) {
        auto la1 = system1->actor_of(props_local, "benchmark_actor" + std::to_string(i));
    }

    system1->stop(true);
}


BOOST_AUTO_TEST_SUITE_END()
