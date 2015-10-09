#define BOOST_TEST_MODULE MAILBOX_TEST

#include <memory>
#include <boost/test/unit_test.hpp>
#include <atan/actor/mailbox/standard_mailbox.h>
#include <atan/actor/mailbox/priority_mailbox.h>
#include <atan/actor/local_actor.h>
#include <atan/actor_system/actor_system.h>
#include <utility.h>
#include "test_actor.h"

using intUptr = std::unique_ptr<int>;
using pMsgIntUptr = priority_message<intUptr>;
using pMsgUptrMsg = priority_message<std::unique_ptr<message>>;

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

    /*
    BOOST_AUTO_TEST_CASE(ActorPriorityMailboxTest) {
        test_actor::message_count = 0;

        auto system1 = actor_system::create_system("test_system1", 4555);

        const actor_ref la1 = local_actor::create<test_actor, priority_mailbox<pMsgUptrMsg>>("test_actor1", system1);

        //task takes 500ms+ to finish
        la1.tell(std::string("msg1"));
        la1.tell(std::string("msg2"));

        //stop the actor after all the messages from the queue are read
        //this will block for 1000ms+
        la1.tell(::stop_actor<int>(5));

        system1->stop(false);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 2);
    }
     */

BOOST_AUTO_TEST_SUITE_END()