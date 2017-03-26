#define BOOST_TEST_MODULE PROPS_TEST

#include <memory>
#include <map>
#include <boost/test/unit_test.hpp>
#include <bumbler/actor/props/typed_props.h>
#include <bumbler/actor/routing/round_robin_pool.h>
#include <bumbler/actor_system/actor_system.h>
#include "test_actor.h"

using namespace bumbler;

BOOST_AUTO_TEST_SUITE(props_test_suite)

BOOST_AUTO_TEST_CASE(PropsLocalActorTest) {
    test_actor::message_count = 0;

    using test_props = typed_props<local_actor, test_actor>;

    auto p = test_props();
    p.with_mailbox<fifo_mailbox>();

    BOOST_CHECK_EQUAL(p.has_router(), false);
    BOOST_CHECK_EQUAL(p.has_mailbox(), true);
    BOOST_CHECK_EQUAL(p.has_network_actor(), false);

    auto system1 = actor_system::create_system("test_system1", 4511);

    actor_ref ref1 = system1->actor_of(p, "actor1");
    ref1.tell(14);

    system1->stop(stop_mode::WAIT_FOR_QUEUE);

    BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);
}

BOOST_AUTO_TEST_CASE(PropsRouterTest) {
    test_actor::message_count = 0;

    using test_props = typed_props<router, test_actor>;

    auto p = test_props();
    p.with_mailbox<fifo_mailbox>().with_router<round_robin_pool>(4);

    BOOST_CHECK_EQUAL(p.has_router(), true);
    BOOST_CHECK_EQUAL(p.has_mailbox(), true);
    BOOST_CHECK_EQUAL(p.has_network_actor(), false);

    auto system1 = actor_system::create_system("test_system1", 4512);

    actor_ref ref1 = system1->actor_of(p, "actor1");
    ref1.tell(14);

    system1->stop(stop_mode::WAIT_FOR_QUEUE);

    BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
