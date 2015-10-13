#define BOOST_TEST_MODULE ROUTER_TEST

#include <boost/test/unit_test.hpp>
#include <atan/actor_system/actor_system.h>
#include <atan/actor/router/round_robin_router.h>
#include <atan/actor/router/random_router.h>
#include <atan/actor/router/smallest_mailbox_router.h>
#include <communication/serializable_types.h>
#include "test_actor.h"
#include "remote_test_actor.h"
#include "test_actor.h"
#include "typed_data.h"

BOOST_AUTO_TEST_SUITE( router_test_suite )
    BOOST_AUTO_TEST_CASE(RouterCreateTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = random_router::create<test_actor>("test_router1", system1, 3);
        const actor_ref r2 = round_robin_router::create<test_actor>("test_router2", system1, 3);

        const actor_ref from_system1 = system1->get_actor("test_router1");
        const actor_ref from_system2 = system1->get_actor("test_router2");

        BOOST_CHECK_EQUAL(from_system1.to_string(), r1.to_string());
        BOOST_CHECK_EQUAL(from_system2.to_string(), r2.to_string());

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterTellTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = round_robin_router::create<test_actor>("test_router1", system1, 2);

        r1.tell(6);

        //wait for the message to get executed
        std::this_thread::sleep_for(std::chrono::seconds(1));

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterSmallestMailboxTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = smallest_mailbox_router::create<test_actor>("test_router1", system1, 2);

        r1.tell(6);
        r1.tell(7);
        r1.tell(8);
        r1.tell(9);

        //wait for the message to get executed
        std::this_thread::sleep_for(std::chrono::seconds(2));

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 4);

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterFutureTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref r1 = round_robin_router::create<test_actor>("test_router1", system1, 3);

        std::future<std::string> f = r1.future<std::string>(6);
        std::future_status s = f.wait_for(std::chrono::seconds(1));

        BOOST_REQUIRE(s == std::future_status::ready);

        BOOST_CHECK_EQUAL(f.get(), "BLAM");

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterTellAllTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555, 1);

        const actor_ref r1 = round_robin_router::create<test_actor>("test_router1", system1, 2);

        r1.tell(broadcast<int>(88));

		r1.tell(typed_data<std::string>(std::string("blam")));

        //wait so the message don't get cleared instantly when the actor_system is shutdown
        std::this_thread::sleep_for(std::chrono::seconds(2));

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 3);

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterRemoteTellAllTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);
        auto system2 = actor_system::create_system("test_system2", 4556);

        const actor_ref test_actor_ref1 = round_robin_router::create<test_actor>("test_actor1", system1, 3);

        actor_ref remote_test_actor1 = remote_actor::create<remote_test_actor>("remote_test_actor1", system2, actor_ref("test_actor1$test_system1@localhost:4555"));

        remote_test_actor1.tell(broadcast<int>(123));

        std::this_thread::sleep_for(std::chrono::seconds(1));

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 3);

        system1->stop(true);
        system2->stop(true);
    }

BOOST_AUTO_TEST_SUITE_END()