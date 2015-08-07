#define BOOST_TEST_MODULE SCHEDULER_TEST

#include <boost/test/unit_test.hpp>
#include <atan/actor/actor_ref.h>
#include <atan/actor_system/actor_system.h>
#include <atan/actor/router/random_router.h>
#include "test_router.h"

void func(std::shared_ptr<dispatcher> disp, actor_ref r1, long initial_delay_ms) {
	auto tm = typed_message<int>(r1, actor_ref::none(), 55);

	std::function<int(int, const typed_message<int>&)> f = [initial_delay_ms](int id, const typed_message<int>& tm) -> int {
		if (initial_delay_ms > 0) {
			std::chrono::milliseconds initial_delay(initial_delay_ms);
			std::this_thread::sleep_for(initial_delay);
		}

		BOOST_LOG_TRIVIAL(debug) << "hello from " << id << ' ' << '\n';
		BOOST_LOG_TRIVIAL(debug) << "[SCHEDULER] thread_id: " << std::this_thread::get_id() <<
			" sending message to " << tm.get_target().to_string();

		tm.get_target().tell(tm);

		return 0;
	};

	disp->push(f, tm);
}

BOOST_AUTO_TEST_SUITE(scheduler_test_suite)

BOOST_AUTO_TEST_CASE(SchedulerSingleMessageTest) {
	test_router::message_count = 0;

	auto system1 = actor_system::create_system("test_system1", 4555);

	const actor_ref r1 = random_router::create<test_router>("test_actor1", system1, 3);

	system1->schedule(55, r1, actor_ref::none(), 0, 0);
//	func(system1->get_dispatcher(), r1, 10);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	BOOST_CHECK_EQUAL(test_router::message_count.load(), 1);

	system1->stop(true);
}

BOOST_AUTO_TEST_CASE(SchedulerBroadcastTest) {
	test_router::message_count = 0;

	auto system1 = actor_system::create_system("test_system1", 4555);

	const actor_ref r1 = random_router::create<test_router>("test_actor1", system1, 3);

	system1->schedule(broadcast<int>(5), r1, 200, 0);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	BOOST_CHECK_EQUAL(test_router::message_count.load(), 3);

	system1->stop(true);
}

BOOST_AUTO_TEST_SUITE_END()