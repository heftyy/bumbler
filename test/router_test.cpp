#define BOOST_TEST_MODULE ROUTER_TEST

#include <boost/test/unit_test.hpp>
#include <communication/serializable_types.h>
#include <bumbler/actor_system/actor_system.h>
#include <bumbler/actor/props/typed_props.h>
#include <bumbler/actor/routing/round_robin_pool.h>
#include <bumbler/actor/routing/smallest_mailbox_pool.h>
#include <bumbler/actor/routing/random_pool.h>
#include <bumbler/messages/message.h>
#include "test_actor.h"
#include "remote_test_actor.h"
#include "test_actor.h"
#include "typed_data.h"

using namespace bumbler;

BOOST_AUTO_TEST_SUITE(router_test_suite)

BOOST_AUTO_TEST_CASE(RouterCreateTest) {
	auto system1 = actor_system::create_system("test_system1", 4513);

	auto p = typed_props<router, test_actor>();
	p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(4);

	auto r1 = system1->actor_of(p, "test_router1");

	auto from_system1 = system1->get_actor_ref("test_router1");

	BOOST_CHECK_EQUAL(from_system1.to_string(), r1.to_string());

	system1->stop(stop_mode::WAIT_FOR_QUEUE);
}

BOOST_AUTO_TEST_CASE(RouterTellTest) {
	test_actor::message_count.store(0);

	auto system1 = actor_system::create_system("test_system1", 4514);

	auto p = typed_props<router, test_actor>();
	p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(1);

	auto r1 = system1->actor_of(p, "test_router1");

	r1.tell(6);

	system1->stop(stop_mode::WAIT_FOR_QUEUE);

	BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);
}

BOOST_AUTO_TEST_CASE(RouterRandomTest) {
	test_actor::message_count.store(0);

	auto system1 = actor_system::create_system("test_system1", 4515);

	auto p = typed_props<router, test_actor>();
	p.with_mailbox<fifo_mailbox>().with_router<random_pool>(2);

	auto r1 = system1->actor_of(p, "test_router1");

	r1.tell(6);
	r1.tell(7);
	r1.tell(8);
	r1.tell(9);

	system1->stop(stop_mode::WAIT_FOR_QUEUE);

	BOOST_CHECK_EQUAL(test_actor::message_count.load(), 4);
}

BOOST_AUTO_TEST_CASE(RouterSmallestMailboxTest) {
	test_actor::message_count.store(0);

	auto system1 = actor_system::create_system("test_system1", 4516);

	auto p = typed_props<router, test_actor>();
	p.with_mailbox<fifo_mailbox>().with_router<smallest_mailbox_pool>(2);

	auto r1 = system1->actor_of(p, "test_router1");

	r1.tell(6);
	r1.tell(7);
	r1.tell(8);
	r1.tell(9);

	system1->stop(stop_mode::WAIT_FOR_QUEUE);

	BOOST_CHECK_EQUAL(test_actor::message_count.load(), 4);
}

BOOST_AUTO_TEST_CASE(RouterFutureTest) {
	test_actor::message_count.store(0);

	auto system1 = actor_system::create_system("test_system1", 4517);

	auto p = typed_props<router, test_actor>();
	p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(3);

	auto r1 = system1->actor_of(p, "test_router1");

	std::future<std::string> f = r1.ask<std::string>(6);
	std::future_status s = f.wait_for(std::chrono::seconds(5));

	BOOST_REQUIRE(s == std::future_status::ready);

	BOOST_CHECK_EQUAL(f.get(), "BLAM");

	BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);

	system1->stop(stop_mode::WAIT_FOR_QUEUE);
}

BOOST_AUTO_TEST_CASE(RouterTellAllTest) {
	test_actor::message_count.store(0);

	auto system1 = actor_system::create_system("test_system1", 4518, 1);

	auto p = typed_props<router, test_actor>();
	p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(2);

	auto r1 = system1->actor_of(p, "test_router1");

	r1.tell(broadcast(88));

	r1.tell(typed_data<std::string>(std::string("blam")));

	system1->stop(stop_mode::WAIT_FOR_QUEUE);

	BOOST_CHECK_EQUAL(test_actor::message_count.load(), 3);
}

BOOST_AUTO_TEST_CASE(RouterRemoteTellAllTest) {
	test_actor::message_count.store(0);

	auto system1 = actor_system::create_system("test_system1", 4519);
	auto system2 = actor_system::create_system("test_system2", 4520);

	auto p = typed_props<router, test_actor>();
	p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(3);

	auto r1 = system1->actor_of(p, "test_router1");

	auto ra1 = actor_ref("test_router1$test_system1@localhost:4519");

	ra1.tell(broadcast(123));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	system1->stop(stop_mode::WAIT_FOR_QUEUE);
	system2->stop(stop_mode::WAIT_FOR_QUEUE);

	BOOST_CHECK_EQUAL(test_actor::message_count.load(), 3);
}

BOOST_AUTO_TEST_SUITE_END()
