#define BOOST_TEST_MODULE ROUTER_TEST

#include <boost/test/unit_test.hpp>
#include <bumbler/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include <bumbler/actor/routing/round_robin_pool.h>
#include <bumbler/actor/routing/smallest_mailbox_pool.h>
#include <bumbler/actor/routing/random_pool.h>
#include "test_actor.h"
#include "remote_test_actor.h"
#include "test_actor.h"
#include "typed_data.h"

BOOST_AUTO_TEST_SUITE( router_test_suite )

    BOOST_AUTO_TEST_CASE(RouterCreateTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        auto p = typed_props<router, test_actor>();
        p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(4);

        auto r1 = system1->actor_of(p, "test_router1");

        const actor_ref from_system1 = system1->get_actor("test_router1");

        BOOST_CHECK_EQUAL(from_system1.to_string(), r1.to_string());

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterTellTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);

        auto p = typed_props<router, test_actor>();
        p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(1);

        auto r1 = system1->actor_of(p, "test_router1");

        r1.tell(6);

        system1->stop(true);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);
    }

	BOOST_AUTO_TEST_CASE(RouterRandomTest) {
		test_actor::message_count.store(0);

		auto system1 = actor_system::create_system("test_system1", 4555);

		auto p = typed_props<router, test_actor>();
		p.with_mailbox<fifo_mailbox>().with_router<random_pool>(2);

		const actor_ref r1 = system1->actor_of(p, "test_router1");

		r1.tell(6);
		r1.tell(7);
		r1.tell(8);
		r1.tell(9);

		system1->stop(true);

		BOOST_CHECK_EQUAL(test_actor::message_count.load(), 4);
	}

    BOOST_AUTO_TEST_CASE(RouterSmallestMailboxTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);

        auto p = typed_props<router, test_actor>();
        p.with_mailbox<fifo_mailbox>().with_router<smallest_mailbox_pool>(2);

        const actor_ref r1 = system1->actor_of(p, "test_router1");

        r1.tell(6);
        r1.tell(7);
        r1.tell(8);
        r1.tell(9);

        system1->stop(true);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 4);
    }

    BOOST_AUTO_TEST_CASE(RouterFutureTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);

        auto p = typed_props<router, test_actor>();
        p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(3);

        const actor_ref r1 = system1->actor_of(p, "test_router1");

        std::future<std::string> f = r1.ask<std::string>(6);
        std::future_status s = f.wait_for(std::chrono::seconds(5));

        BOOST_REQUIRE(s == std::future_status::ready);

        BOOST_CHECK_EQUAL(f.get(), "BLAM");

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(RouterTellAllTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555, 1);

        auto p = typed_props<router, test_actor>();
        p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(2);

        const actor_ref r1 = system1->actor_of(p, "test_router1");

        r1.tell(broadcast<int>(88));

		r1.tell(typed_data<std::string>(std::string("blam")));

        system1->stop(true);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 3);
    }

    BOOST_AUTO_TEST_CASE(RouterRemoteTellAllTest) {
        test_actor::message_count.store(0);

        auto system1 = actor_system::create_system("test_system1", 4555);
        auto system2 = actor_system::create_system("test_system2", 4556);

        auto p = typed_props<router, test_actor>();
        p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(3);

        auto r1 = system1->actor_of(p, "test_router1");

        auto props_remote = typed_props<remote_actor, remote_test_actor>();
        props_remote.with_network_actor("test_router1$test_system1@localhost:4555");
        auto ra1 = system2->actor_of(props_remote, "remote_test_actor1");

        ra1.tell(broadcast<int>(123));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        system1->stop(true);
        system2->stop(true);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 3);
    }

BOOST_AUTO_TEST_SUITE_END()