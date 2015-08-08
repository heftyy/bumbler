#define BOOST_TEST_MODULE ROUTER_TEST

#include <boost/test/unit_test.hpp>
#include <atan/actor/actor_ref.h>
#include <atan/actor_system/actor_system.h>
#include <atan/actor/router/round_robin_router.h>
#include <atan/actor/router/random_router.h>
#include "test_actor.h"
#include "remote_test_actor.h"
#include "test_router.h"
#include "typed_data.h"

BOOST_AUTO_TEST_SUITE( router_test_suite )
    BOOST_AUTO_TEST_CASE(RouterCreateTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = random_router::create<test_router>("test_router1", system1, 3);
        const actor_ref r2 = round_robin_router::create<test_router>("test_router2", system1, 3);

        const actor_ref from_system1 = system1->get_actor("test_router1");
        const actor_ref from_system2 = system1->get_actor("test_router2");

        BOOST_CHECK_EQUAL(from_system1.to_string(), r1.to_string());
        BOOST_CHECK_EQUAL(from_system2.to_string(), r2.to_string());

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterTellTest) {
        test_router::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = round_robin_router::create<test_router>("test_router1", system1, 2);

        r1.tell(6);

        //wait for the message to get executed
        std::this_thread::sleep_for(std::chrono::seconds(1));

        BOOST_CHECK_EQUAL(test_router::message_count.load(), 1);

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterFutureTest) {
        test_router::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = round_robin_router::create<test_router>("test_router1", system1, 3);

        std::future<std::string> f = r1.future<std::string>(6);
        std::future_status s = f.wait_for(std::chrono::seconds(1));

        BOOST_REQUIRE(s == std::future_status::ready);

        BOOST_CHECK_EQUAL(f.get(), "BLAM");

        BOOST_CHECK_EQUAL(test_router::message_count.load(), 1);

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterTellAllTest) {
        test_router::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555, 1);

        const actor_ref r1 = round_robin_router::create<test_router>("test_router1", system1, 2);

        r1.tell(broadcast<int>(88));

		r1.tell(typed_data<std::string>("blam"));

        //wait so the message don't get cleared instantly when the actor_system is shutdown
        std::this_thread::sleep_for(std::chrono::seconds(2));

        BOOST_CHECK_EQUAL(test_router::message_count.load(), 3);

        system1->stop(true);
    }

BOOST_AUTO_TEST_SUITE_END()