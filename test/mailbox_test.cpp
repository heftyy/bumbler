#define BOOST_TEST_MODULE MAILBOX_TEST

#include <memory>
#include <boost/test/unit_test.hpp>
#include <atan/actor/mailbox/standard_mailbox.h>
#include <atan/actor/mailbox/priority_mailbox.h>
#include <utility.h>

using intUptr = std::unique_ptr<int>;
using pMsgIntUptr = priority_message<intUptr>;

BOOST_AUTO_TEST_SUITE( mailbox_test_suite )

    BOOST_AUTO_TEST_CASE(StandardMailboxTest) {
        std::unique_ptr<mailbox<int>> mailbox = utility::make_unique<standard_mailbox<int>>();

        mailbox->push_message(5);
        mailbox->push_message(6);
        mailbox->push_message(7);

        BOOST_CHECK_EQUAL(5, mailbox->pop_message());
        BOOST_CHECK_EQUAL(6, mailbox->pop_message());
        BOOST_CHECK_EQUAL(7, mailbox->pop_message());
    }

    BOOST_AUTO_TEST_CASE(StandardMailboxTestUniquePtr) {
        std::unique_ptr<mailbox<intUptr>> mailbox = utility::make_unique<standard_mailbox<intUptr>>();

        mailbox->push_message(intUptr(new int(5)));
        mailbox->push_message(intUptr(new int(6)));
        mailbox->push_message(intUptr(new int(7)));

        BOOST_CHECK_EQUAL(5, *mailbox->pop_message());
        BOOST_CHECK_EQUAL(6, *mailbox->pop_message());
        BOOST_CHECK_EQUAL(7, *mailbox->pop_message());
    }

    BOOST_AUTO_TEST_CASE(PriorityMailboxTest) {
        std::unique_ptr<mailbox<priority_message<int>>> mailbox = utility::make_unique<priority_mailbox<priority_message<int>>>();

        mailbox->push_message(priority_message<int>(5, 1));
        mailbox->push_message(priority_message<int>(6, 2));
        mailbox->push_message(priority_message<int>(7, 3));

        BOOST_CHECK_EQUAL(7, mailbox->pop_message().message);
        BOOST_CHECK_EQUAL(6, mailbox->pop_message().message);
        BOOST_CHECK_EQUAL(5, mailbox->pop_message().message);
    }

    BOOST_AUTO_TEST_CASE(PriorityMailboxTestUniquePtr) {
        std::unique_ptr<mailbox<pMsgIntUptr>> mailbox = utility::make_unique<priority_mailbox<pMsgIntUptr>>();

        mailbox->push_message(pMsgIntUptr(intUptr(new int(5)), 1));
        mailbox->push_message(pMsgIntUptr(intUptr(new int(6)), 2));
        mailbox->push_message(pMsgIntUptr(intUptr(new int(7)), 3));

        BOOST_CHECK_EQUAL(7, *mailbox->pop_message().message);
        BOOST_CHECK_EQUAL(6, *mailbox->pop_message().message);
        BOOST_CHECK_EQUAL(5, *mailbox->pop_message().message);
    }

BOOST_AUTO_TEST_SUITE_END()