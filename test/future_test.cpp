#define BOOST_TEST_MODULE FUTURE_TEST

#include <sstream>

#include <boost/test/unit_test.hpp>
#include <bumbler/actor/props/typed_props.h>
#include <bumbler/actor_system/actor_system.h>
#include <communication/serializable_types.h>
#include <bumbler/actor/local_actor.h>
#include <bumbler/messages/typed_variant.h>
#include "test_actor.h"
#include "remote_test_actor.h"
#include "typed_data.h"

using namespace bumbler;

BOOST_CLASS_EXPORT(typed_data<int>)
BOOST_CLASS_EXPORT(typed_variant<typed_data<int>>)

BOOST_AUTO_TEST_SUITE(future_test_suite)

BOOST_AUTO_TEST_CASE(ActorLocalFutureTest) {
    auto system1 = actor_system::create_system("test_system1", 4505);

    auto props_local = typed_props<local_actor, test_actor>();
    auto la1 = system1->actor_of(props_local, "test_actor1");

    actor_ref from_system1 = system1->get_actor_ref("test_actor1");
    BOOST_CHECK_EQUAL(from_system1.to_string(), la1.to_string());

    std::future<std::string> f1 = la1.ask<std::string>(1);
    auto status1 = f1.wait_for(std::chrono::seconds(5));
    BOOST_REQUIRE(status1 == std::future_status::ready);

    BOOST_CHECK_EQUAL(f1.get(), "BLAM");

    system1->stop(stop_mode::WAIT_FOR_QUEUE);
}

BOOST_AUTO_TEST_CASE(ActorRemoteFutureTest) {
    auto system1 = actor_system::create_system("test_system1", 4506);

    auto props_local = typed_props<local_actor, test_actor>();

    auto la1 = system1->actor_of(props_local, "test_actor1");
    auto ra1 = actor_ref("test_actor1$test_system1@localhost:4506");

    priority_message pm(5, 10);

    std::future<std::string> f1 = ra1.ask<std::string>(pm);
    auto status1 = f1.wait_for(std::chrono::seconds(5));
    BOOST_REQUIRE(status1 == std::future_status::ready);

    BOOST_CHECK_EQUAL(f1.get(), "BLAM");

    system1->stop(stop_mode::WAIT_FOR_QUEUE);
}

BOOST_AUTO_TEST_CASE(ActorRemoteFutureSerializationTest) {
    auto system1 = actor_system::create_system("test_system1", 4508);

    auto props_local = typed_props<local_actor, test_actor>();

    auto la1 = system1->actor_of(props_local, "test_actor1");
    auto ra1 = actor_ref("test_actor1$test_system1@localhost:4508");

    std::future<std::string> f1 = ra1.ask<std::string>(typed_data<int>(66));
    auto status1 = f1.wait_for(std::chrono::seconds(5));
    BOOST_REQUIRE(status1 == std::future_status::ready);

    BOOST_CHECK_EQUAL(f1.get(), "BLAM");

    system1->stop(stop_mode::WAIT_FOR_QUEUE);
}

BOOST_AUTO_TEST_SUITE_END()
