#define BOOST_TEST_MODULE MAILBOX_TEST

#include <memory>
#include <boost/test/unit_test.hpp>
#include <atan/actor/router/mailbox/priority_mailbox.h>

BOOST_AUTO_TEST_SUITE( mailbox_test_suite )

    BOOST_AUTO_TEST_CASE(PriorityMailboxTest) {
        auto mailbox = priority_mailbox<std::shared_ptr<int>>();
    }

BOOST_AUTO_TEST_SUITE_END()