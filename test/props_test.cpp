#define BOOST_TEST_MODULE MESSAGE_TEST

#include <memory>
#include <map>
#include <boost/test/unit_test.hpp>
#include <atan/actor/props/typed_props.h>
#include <atan/actor/routing/router.h>
#include <atan/actor/routing/round_robin_pool.h>
#include <atan/actor_system/actor_system.h>
#include <utility.h>
#include "test_actor.h"

BOOST_AUTO_TEST_SUITE(props_test_suite)

    BOOST_AUTO_TEST_CASE(PropsLocalActorTest) {
        test_actor::message_count = 0;

        using test_props = typed_props<local_actor, test_actor>;
        using test_props_ptr = std::unique_ptr<test_props>;
        using props_ptr = std::unique_ptr<props>;

        props_ptr p = utility::make_unique<test_props>();
        p->with_mailbox(utility::make_unique<fifo_mailbox>());

        BOOST_CHECK_EQUAL(p->has_router(), false);
        BOOST_CHECK_EQUAL(p->has_mailbox(), true);

        auto system1 = actor_system::create_system("test_system1", 4555);

        actor_ref ref1 = system1->actor_of(p, "actor1");

        ref1.tell(14);

        system1->stop(true);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);
    }

    BOOST_AUTO_TEST_CASE(PropsRouterTest) {
        test_actor::message_count = 0;

        using test_props = typed_props<router, test_actor>;
        using test_props_ptr = std::unique_ptr<test_props>;
        using props_ptr = std::unique_ptr<props>;

        props_ptr p = utility::make_unique<test_props>();
        p->with_router(utility::make_unique<round_robin_pool>(4)).with_mailbox(utility::make_unique<fifo_mailbox>());

        BOOST_CHECK_EQUAL(p->has_router(), true);
        BOOST_CHECK_EQUAL(p->has_mailbox(), true);

        auto system1 = actor_system::create_system("test_system1", 4555);

        actor_ref ref1 = system1->actor_of(p, "actor1");

//        BOOST_CHECK_NE(actor_ref::none(), ref1);
//        BOOST_CHECK(ref1.is_none() == false);

        ref1.tell(14);
//        ref1.tell(15);
//        ref1.tell(16);
//        ref1.tell(17);

        system1->stop(true);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);
    }

BOOST_AUTO_TEST_SUITE_END()