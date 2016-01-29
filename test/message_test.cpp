#define BOOST_TEST_MODULE MESSAGE_TEST

#include <memory>
#include <map>
#include <boost/test/unit_test.hpp>
#include <bumbler/messages/typed_message.h>
#include <bumbler/actor/actor_ref/actor_ref.h>
#include <bumbler/utility.h>

using namespace bumbler;

using intMsg = typed_message<int>;

BOOST_AUTO_TEST_SUITE( message_test_suite )

    BOOST_AUTO_TEST_CASE(IntMessageTest) {
        auto target = actor_ref("target_actor", "target_system");
        auto sender = actor_ref("sender_actor", "sender_system");

        auto tm = typed_message_factory::create(target, sender, 11);

        {
            bool is_same = std::is_same<decltype(tm), intMsg>::value;
            BOOST_CHECK(is_same);

            BOOST_CHECK_EQUAL(tm.data, 11);

            boost::any data = tm.get_data();
            BOOST_CHECK_EQUAL(boost::any_cast<int>(data), 11);

            BOOST_CHECK(target == tm.get_target());
            BOOST_CHECK(sender == tm.get_sender());
        }

        {
            auto tm_copy = tm;

            bool is_same = std::is_same<decltype(tm), decltype(tm_copy)>::value;
            BOOST_CHECK(is_same);

            BOOST_CHECK_EQUAL(tm_copy.data, 11);

            boost::any data = tm_copy.get_data();
            BOOST_CHECK_EQUAL(boost::any_cast<int>(data), 11);

            BOOST_CHECK(target == tm_copy.get_target());
            BOOST_CHECK(sender == tm_copy.get_sender());
        }

        {
            auto tm_copy(tm);

            bool is_same = std::is_same<decltype(tm), decltype(tm_copy)>::value;
            BOOST_CHECK(is_same);

            BOOST_CHECK_EQUAL(tm_copy.data, 11);

            boost::any data = tm_copy.get_data();
            BOOST_CHECK_EQUAL(boost::any_cast<int>(data), 11);

            BOOST_CHECK(target == tm_copy.get_target());
            BOOST_CHECK(sender == tm_copy.get_sender());
        }
    }

BOOST_AUTO_TEST_SUITE_END()
