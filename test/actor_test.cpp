#define BOOST_TEST_MODULE ACTOR_TEST

#include <boost/test/unit_test.hpp>
#include <atan/actor/actor_ref.h>
#include <atan/actor_system/actor_system.h>
#include <atan/actor/local_actor.h>
#include "test_actor.h"
#include "remote_test_actor.h"

BOOST_AUTO_TEST_SUITE( actor_test_suite )

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

    BOOST_AUTO_TEST_CASE(ActorSystemInitTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);
        BOOST_CHECK_EQUAL(system1->started(), true);

        BOOST_CHECK_EQUAL(system1->stopped(), false);
		system1->stop(false);
        BOOST_CHECK_EQUAL(system1->stopped(), true);
    }


    BOOST_AUTO_TEST_CASE(ActorInitTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref la1 = local_actor::create<test_actor>("test_actor1", system1);
        const actor_ref ra1 = remote_actor::create<remote_test_actor>("remote_test_actor1", system1, actor_ref("test_actor1$test_system1@localhost:4555"));

        actor_ref from_system1 = system1->get_actor("test_actor1");
        actor_ref from_system2 = system1->get_actor("remote_test_actor1");

        BOOST_CHECK_EQUAL(from_system1.to_string(), la1.to_string());
        BOOST_CHECK_EQUAL(from_system2.to_string(), ra1.to_string());

        system1->stop(false);
    }

    BOOST_AUTO_TEST_CASE(ActorStopTest) {
        test_actor::message_count = 0;

        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref la1 = local_actor::create<test_actor>("test_actor1", system1);

        //task takes 500ms+ to finish
        la1.tell(std::string("msg1"));
        la1.tell(std::string("msg2"));

        //stop the actor after all the messages from the queue are read
        //this will block for 1000ms+
        la1.tell(::stop_actor<int>(5));

        system1->stop(false);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 2);
    }

    BOOST_AUTO_TEST_CASE(ActorKillTest) {
        test_actor::message_count = 0;

        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref la1 = local_actor::create<test_actor>("test_actor1", system1);

        //task takes 500ms+ to finish
        la1.tell(std::string("msg1"));
        la1.tell(std::string("msg2"));

        //wait a bit so both the messages are added to the queue
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //clear the queue ( 1 message left ) and stop the actor
        la1.tell(::kill_actor<int>(5));

        system1->stop(false);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 1);
    }

BOOST_AUTO_TEST_SUITE_END()