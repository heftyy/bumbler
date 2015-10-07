#define BOOST_TEST_MODULE MAILBOX_TEST

#include <memory>
#include <boost/test/unit_test.hpp>
#include <atan/actor/mailbox/standard_mailbox.h>
#include <atan/actor/mailbox/priority_mailbox.h>

BOOST_AUTO_TEST_SUITE( mailbox_test_suite )

    BOOST_AUTO_TEST_CASE(StandardMailboxTest) {
        auto mailbox = standard_mailbox<int>();

        int msg_before = 5;

        BOOST_CHECK_EQUAL(5, msg_before);

        mailbox.push_message(msg_before);

        int msg_after = mailbox.pop_message();

        BOOST_CHECK_EQUAL(5, msg_after);
    }

    BOOST_AUTO_TEST_CASE(StandardMailboxTestUnPtr) {
        auto mailbox = standard_mailbox<std::unique_ptr<int>>();

        auto msg_before = std::unique_ptr<int>(new int(5));

        BOOST_CHECK_EQUAL(5, *msg_before);

        mailbox.push_message(std::move(msg_before));

        auto msg_after = mailbox.pop_message();

        BOOST_CHECK_EQUAL(5, *msg_after);
    }

    BOOST_AUTO_TEST_CASE(PriorityMailboxTest) {
        auto mailbox = priority_mailbox<std::shared_ptr<int>>();
    }

BOOST_AUTO_TEST_SUITE_END()