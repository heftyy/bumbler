#define BOOST_TEST_MODULE MESSAGE_TEST

#include <memory>
#include <map>
#include <boost/test/unit_test.hpp>
#include <atan/actor/props/typed_props.h>
#include <atan/actor/routing/router.h>
#include <atan/actor/routing/round_robin_pool.h>
#include <utility.h>
#include "test_actor.h"

using test_props = typed_props<router, test_actor, int>;
using test_props_ptr = std::unique_ptr<test_props>;
using props_ptr = std::unique_ptr<props>;

BOOST_AUTO_TEST_SUITE(props_test_suite)

    BOOST_AUTO_TEST_CASE(InitPropsTest) {
        test_props_ptr p = utility::make_unique<test_props>(5);
        p->with_router(utility::make_unique<round_robin_pool>(4)).with_mailbox(utility::make_unique<fifo_mailbox>());
    }

BOOST_AUTO_TEST_SUITE_END()