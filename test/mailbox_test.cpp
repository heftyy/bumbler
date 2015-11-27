#define BOOST_TEST_MODULE MAILBOX_TEST

#include <memory>
#include <map>
#include <boost/test/unit_test.hpp>
#include <atan/actor/mailbox/priority_mailbox.h>
#include <atan/actor/mailbox/fifo_mailbox.h>
#include <atan/actor/local_actor.h>
#include <atan/actor_system/actor_system.h>
#include <utility.h>
#include "test_actor.h"

using msgPtr = std::unique_ptr<message>;
using priorityMsg = priority_message<int>;

BOOST_AUTO_TEST_SUITE( mailbox_test_suite )

    BOOST_AUTO_TEST_CASE(StandardMailboxTest) {
        std::unique_ptr<mailbox> mailbox = utility::make_unique<fifo_mailbox>();

        auto tm = typed_message_factory::create(actor_ref::none(), actor_ref::none(), 5);
        auto tm_ptr = utility::make_unique<decltype(tm)>(std::move(tm));

        mailbox->push_message(std::move(tm_ptr));

        BOOST_CHECK_EQUAL(mailbox->empty(), false);

        auto result = mailbox->pop_message();
        BOOST_CHECK_EQUAL(result->get_data().type().hash_code(), typeid(int).hash_code());
        BOOST_CHECK_EQUAL(boost::any_cast<int>(result->get_data()), 5);
        BOOST_CHECK_EQUAL(mailbox->empty(), true);
    }

    BOOST_AUTO_TEST_CASE(PriorityMailboxTest) {
        std::unique_ptr<mailbox> mailbox = utility::make_unique<priority_mailbox>();

        // add one message without a priority set ( uses default 0 )
        auto tm = typed_message_factory::create(actor_ref::none(), actor_ref::none(), 4);
        auto tm_ptr = utility::make_unique<decltype(tm)>(std::move(tm));

        mailbox->push_message(std::move(tm_ptr));

        const std::map<int, int> map {
                {5, 100},
                {6, 101},
                {7, 102},
                {8, 103}
        };

        for(auto const &entry : map) {
            // add 4 messages with priority from the map
            auto tm = typed_message_factory::create(actor_ref::none(), actor_ref::none(), priority_message<int>(entry.first, entry.second));
            auto tm_ptr = utility::make_unique<decltype(tm)>(std::move(tm));

            mailbox->push_message(std::move(tm_ptr));
        }

        BOOST_CHECK_EQUAL(mailbox->empty(), false);

        auto result1 = mailbox->pop_message();
        BOOST_CHECK_EQUAL(result1->get_data().type().hash_code(), typeid(int).hash_code());
        BOOST_CHECK_EQUAL(boost::any_cast<int>(result1->get_data()), 8);

        auto result2 = mailbox->pop_message();
        BOOST_CHECK_EQUAL(result2->get_data().type().hash_code(), typeid(int).hash_code());
        BOOST_CHECK_EQUAL(boost::any_cast<int>(result2->get_data()), 7);

        auto result3 = mailbox->pop_message();
        BOOST_CHECK_EQUAL(result3->get_data().type().hash_code(), typeid(int).hash_code());
        BOOST_CHECK_EQUAL(boost::any_cast<int>(result3->get_data()), 6);

        auto result4 = mailbox->pop_message();
        BOOST_CHECK_EQUAL(result4->get_data().type().hash_code(), typeid(int).hash_code());
        BOOST_CHECK_EQUAL(boost::any_cast<int>(result4->get_data()), 5);

        auto result5 = mailbox->pop_message();
        BOOST_CHECK_EQUAL(result5->get_data().type().hash_code(), typeid(int).hash_code());
        BOOST_CHECK_EQUAL(boost::any_cast<int>(result5->get_data()), 4);

        BOOST_CHECK_EQUAL(mailbox->empty(), true);
    }

    BOOST_AUTO_TEST_CASE(ActorPriorityMailboxTest) {
        test_actor::message_count = 0;

        auto system1 = actor_system::create_system("test_system1", 4555);

        auto props_local = typed_props<local_actor, test_actor>();
        props_local.with_mailbox<priority_mailbox>();
        const actor_ref la1 = system1->actor_of(props_local, "test_actor1");

        // task takes 500ms+ to finish
        la1.tell(std::string("regular msg1"));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        la1.tell(priority_message<std::string>("priority msg second", 5));
        la1.tell(priority_message<std::string>("priority msg third ", 4));
        la1.tell(priority_message<std::string>("priority msg first", 9));

        // stop the actor after all the messages from the queue are read
        // this will block for 1000ms+
        la1.tell(::stop_actor<int>(5));

        system1->stop(false);

        BOOST_CHECK_EQUAL(test_actor::message_count.load(), 4);
    }

BOOST_AUTO_TEST_SUITE_END()