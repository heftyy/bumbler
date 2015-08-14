#define BOOST_TEST_MODULE SCHEDULER_TEST

#include <boost/test/unit_test.hpp>
#include <atan/actor_system/actor_system.h>
#include <atan/actor/router/round_robin_router.h>
#include "test_actor.h"
#include "typed_data.h"

BOOST_AUTO_TEST_SUITE(scheduler_test_suite)

BOOST_AUTO_TEST_CASE(SchedulerSingleMessageTest) {
	test_actor::message_count = 0;

	auto system1 = actor_system::create_system("test_system1", 4555);

	auto&& r1 = round_robin_router::create<test_actor>("test_actor1", system1, 3);

	auto blam1 = system1->schedule(55, r1, actor_ref::none(), 0, 0);
	auto blam2 = system1->schedule(54, r1, actor_ref::none(), 100, 0);

	//cancel before the message is ever sent
	auto c1 = system1->schedule(53, r1, actor_ref::none(), 500, 0 );
	c1->cancel();

	//cancel after the message is sent 4 times (0/300/600/900 ms)
 	auto c2 = system1->schedule(52, r1, actor_ref::none(), 0, 300);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	c2.reset();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1+1+4);

	system1->stop(false);
}

BOOST_AUTO_TEST_CASE(SchedulerBroadcastTest) {
	test_actor::message_count = 0;

	auto system1 = actor_system::create_system("test_system1", 4555);

	auto&& r1 = round_robin_router::create<test_actor>("test_actor1", system1, 3);

	auto c1 = system1->schedule(::broadcast<int>(5), r1, actor_ref::none(), 200, 0);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	BOOST_CHECK_EQUAL(test_actor::message_count.load(), 3);

	system1->stop(true);
}

BOOST_AUTO_TEST_SUITE_END()