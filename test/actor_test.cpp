#define BOOST_TEST_MODULE ACTOR_TEST

#include <memory>
#include <future>
#include <boost/test/unit_test.hpp>
#include <bumbler/actor/props/typed_props.h>
#include <bumbler/thread_pool/thread_pool.h>
#include <bumbler/actor_system/actor_system.h>
#include <bumbler/actor/local_actor.h>
#include "test_actor.h"

using namespace bumbler;

BOOST_AUTO_TEST_SUITE(actor_suite)

BOOST_AUTO_TEST_CASE(ActorSystemInitTest) {
    auto system1 = actor_system::create_system("test_system1", 4555);
    BOOST_CHECK_EQUAL(system1->started(), true);

    BOOST_CHECK_EQUAL(system1->stopped(), false);
    system1->stop(false);
    BOOST_CHECK_EQUAL(system1->stopped(), true);
}

BOOST_AUTO_TEST_CASE(ActorInitTest) {
    auto system1 = actor_system::create_system("test_system1", 4555);

    auto props_local = typed_props<local_actor, test_actor>();

    auto la1 = system1->actor_of(props_local, "test_actor1");

    auto from_system1 = system1->get_actor_ref("test_actor1");

    BOOST_CHECK_EQUAL(from_system1.to_string(), la1.to_string());

    system1->stop(false);
}

BOOST_AUTO_TEST_CASE(ActorStopTest) {
    test_actor::message_count = 0;

    auto system1 = actor_system::create_system("test_system1", 4555);

    auto props_local = typed_props<local_actor, test_actor>();
    auto la1 = system1->actor_of(props_local, "test_actor1");

    //task takes 500ms+ to finish
    la1.tell(std::string("msg1"));
    la1.tell(std::string("msg2"));

    //stop the actor after all the messages from the queue are read
    //this will block for 1000ms+
    la1.tell(bumbler::stop_actor<int>(5));

    system1->stop(false);

    BOOST_CHECK_EQUAL(test_actor::message_count.load(), 2);
}

BOOST_AUTO_TEST_CASE(ActorKillTest) {
    test_actor::message_count = 0;

    auto system1 = actor_system::create_system("test_system1", 4555);

    auto props_local = typed_props<local_actor, test_actor>();
    auto la1 = system1->actor_of(props_local, "test_actor1");

    //task takes 500ms+ to finish
    la1.tell(std::string("msg1"));
    la1.tell(std::string("msg2"));

    //clear the queue ( 1 message left ) and stop the actor
    la1.tell(bumbler::kill_actor < int > (5));

    system1->stop(false);

    BOOST_CHECK_LE(test_actor::message_count.load(), 1);
}


BOOST_AUTO_TEST_SUITE_END()
