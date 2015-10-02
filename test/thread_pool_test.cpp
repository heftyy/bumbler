#define BOOST_TEST_MODULE THREAD_POOL_TEST

#include <memory>
#include <future>
#include <boost/test/unit_test.hpp>
#include <atan/thread_pool/thread_pool.h>
#include <logger/logger.h>

BOOST_AUTO_TEST_SUITE( thread_pool_suite )

    BOOST_AUTO_TEST_CASE(ThreadPoolTest) {
        thread_pool tp(2);

        int task_counter = 0;

        tp.push([&task_counter](int x) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            BOOST_LOG_TRIVIAL(debug) << "pck task " << x;
            task_counter++;
        }, 1);

        tp.push([&task_counter](int x) {
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            BOOST_LOG_TRIVIAL(debug) << "pck task " << x;
            task_counter++;
        }, 2);

        tp.push([&task_counter](int x) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            BOOST_LOG_TRIVIAL(debug) << "pck task " << x;
            task_counter++;
        }, 3);

        tp.push([&task_counter](int x) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            BOOST_LOG_TRIVIAL(debug) << "pck task " << x;
            task_counter++;
        }, 4);

        //wait for all tasks to finish
        tp.stop(true);

        BOOST_CHECK_EQUAL(task_counter, 4);
    }

BOOST_AUTO_TEST_SUITE_END()