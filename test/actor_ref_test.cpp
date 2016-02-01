#define BOOST_TEST_MODULE ACTOR_REF_TEST

#include <memory>
#include <future>
#include <boost/test/unit_test.hpp>
#include <bumbler/actor/local_actor.h>
#include <bumbler/actor/actor_ref/actor_ref.h>
#include <bumbler/actor_system/actor_system.h>
#include "test_actor.h"

using namespace bumbler;

BOOST_AUTO_TEST_SUITE(actor_suite)

BOOST_AUTO_TEST_CASE(ActorRefTest) {
    actor_ref test_ref = actor_ref("test_actor", "test_system");

    BOOST_CHECK_EQUAL(test_ref.actor_name, "test_actor");
    BOOST_CHECK_EQUAL(test_ref.system_name, "test_system");

    actor_ref none_ref = actor_ref::none();

    BOOST_CHECK(none_ref.is_none());

    actor_ref remote_test_ref = actor_ref("test_actor", "test_system", "localhost", 4555);
    BOOST_CHECK(remote_test_ref.is_remote());

    BOOST_CHECK_EQUAL(remote_test_ref.to_string(), "test_actor$test_system@localhost:4555");
}

BOOST_AUTO_TEST_CASE(ActorRefResolveTest) {
    using test_props = typed_props<local_actor, test_actor>;
    auto p = test_props();

    auto system1 = actor_system::create_system("test_system1", 4555);
    auto ref1 = system1->actor_of(p, "actor1");

    ref1.resolve();
    BOOST_CHECK_EQUAL(ref1.is_resolved(), true);

    system1->stop(false);
}

BOOST_AUTO_TEST_SUITE_END()