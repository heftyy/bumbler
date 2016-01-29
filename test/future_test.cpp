#define BOOST_TEST_MODULE FUTURE_TEST

#include <boost/test/unit_test.hpp>
#include <bumbler/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include <bumbler/actor/local_actor.h>
#include "test_actor.h"
#include "remote_test_actor.h"
#include "typed_data.h"

using namespace bumbler;

BOOST_CLASS_EXPORT(typed_message<typed_data<int>>)

BOOST_AUTO_TEST_SUITE( future_test_suite )

    BOOST_AUTO_TEST_CASE(ActorLocalFutureTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);

        auto props_local = typed_props<local_actor, test_actor>();
        const actor_ref la1 = system1->actor_of(props_local, "test_actor1");

        actor_ref from_system1 = system1->get_actor("test_actor1");
        BOOST_CHECK_EQUAL(from_system1.to_string(), la1.to_string());

        std::future<std::string> f1 = la1.ask<std::string>(1);
        auto status1 = f1.wait_for(std::chrono::seconds(5));
        BOOST_REQUIRE(status1 == std::future_status::ready);

        BOOST_CHECK_EQUAL(f1.get(), "BLAM");

        system1->stop(true);
    }

    BOOST_AUTO_TEST_CASE(ActorRemoteFutureTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);
        auto system2 = actor_system::create_system("test_system2", 4556);

        auto props_local = typed_props<local_actor, test_actor>();
        auto props_remote = typed_props<remote_actor, test_actor>();
        props_remote.with_network_actor("test_actor1$test_system1@localhost:4555");

        const actor_ref la1 = system1->actor_of(props_local, "test_actor1");
        const actor_ref ra1 = system2->actor_of(props_remote, "remote_test_actor1");

        std::future<std::string> f1 = ra1.ask<std::string>(priority_message<int>(5, 10));
        auto status1 = f1.wait_for(std::chrono::seconds(5));
        BOOST_REQUIRE(status1 == std::future_status::ready);

        BOOST_CHECK_EQUAL(f1.get(), "BLAM");

        system1->stop(true);
        system2->stop(true);
    }

    BOOST_AUTO_TEST_CASE(ActorRemoteFutureSerializationTest) {
        auto system1 = actor_system::create_system("test_system1", 4555);
        auto system2 = actor_system::create_system("test_system2", 4556);

        auto props_local = typed_props<local_actor, test_actor>();
        auto props_remote = typed_props<remote_actor, test_actor>();
        props_remote.with_network_actor("test_actor1$test_system1@localhost:4555");

        const actor_ref la1 = system1->actor_of(props_local, "test_actor1");
        const actor_ref ra1 = system2->actor_of(props_remote, "remote_test_actor1");

        std::future<std::string> f1 = ra1.ask<std::string>(typed_data<int>(66));
        auto status1 = f1.wait_for(std::chrono::seconds(5));
        BOOST_REQUIRE(status1 == std::future_status::ready);

        BOOST_CHECK_EQUAL(f1.get(), "BLAM");

        system1->stop(true);
        system2->stop(true);
    }

BOOST_AUTO_TEST_SUITE_END()
