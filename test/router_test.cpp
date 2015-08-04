#define BOOST_TEST_MODULE ROUTER_TEST

#include <boost/test/unit_test.hpp>
#include <atan/actor/actor_ref.h>
#include <atan/actor_system/actor_system.h>
#include "test_actor.h"
#include "remote_test_actor.h"
#include "test_router.h"

BOOST_AUTO_TEST_SUITE( router_test_suite )

    BOOST_AUTO_TEST_CASE(RouterCreateTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = actor::create_router<test_router>("test_router1", system1);

        const actor_ref from_system1 = system1->get_actor("test_router1");

        BOOST_CHECK_EQUAL(from_system1.to_string(), r1.to_string());

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterTellTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = actor::create_router<test_router>("test_router1", system1);

        r1.tell(6);

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterFutureTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = actor::create_router<test_router>("test_router1", system1);

        std::future<std::string> f = r1.future<std::string>(6);
        std::future_status s = f.wait_for(std::chrono::seconds(1));

        BOOST_REQUIRE(s == std::future_status::ready);

        BOOST_CHECK_EQUAL(f.get(), "BLAM");

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterTellAllTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = actor::create_router<test_router>("test_router1", system1);

        r1.tell(broadcast<int>(88));

        system1->stop(true);
    }

BOOST_AUTO_TEST_SUITE_END()