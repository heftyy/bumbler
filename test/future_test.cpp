#define BOOST_TEST_MODULE FUTURE_TEST

#include <boost/test/unit_test.hpp>
#include <atan/actor/actor_ref.h>
#include <atan/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include "test_actor.h"
#include "remote_test_actor.h"

BOOST_AUTO_TEST_SUITE( future_test_suite )

    BOOST_AUTO_TEST_CASE(ActorLocalFutureTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref test_actor_ref1 = actor::create_actor<test_actor>("test_actor1", system1);

        std::future<std::string> f1 = test_actor_ref1.future<std::string>(1);
        auto status1 = f1.wait_for(std::chrono::seconds(1));
        BOOST_REQUIRE(status1 == std::future_status::ready);

        BOOST_CHECK_EQUAL(f1.get(), "BLAM");

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(ActorRemoteFutureTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);
        auto system2 = actor_system::create_system("test_system2", 4556);

        const actor_ref test_actor_ref1 = actor::create_actor<test_actor>("test_actor1", system1);
        const actor_ref test_actor_ref2 = actor::create_actor<test_actor>("test_actor2", system2);

        actor_ref remote_test_actor1 = actor::create_actor<remote_test_actor>("remote_test_actor1", system1, "test_actor2$test_system2@localhost:4556");

        std::future<std::string> f1 = remote_test_actor1.future<std::string>(1);
        auto status1 = f1.wait_for(std::chrono::seconds(1));
        BOOST_REQUIRE(status1 == std::future_status::ready);

        BOOST_CHECK_EQUAL(f1.get(), "BLAM");

        system1->stop(true);
        system2->stop(true);
    }

BOOST_AUTO_TEST_SUITE_END()