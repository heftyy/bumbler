#define BOOST_TEST_MODULE THREAD_POOL_TEST

#include <memory>
#include <future>
#include <boost/test/unit_test.hpp>
#include <bumbler/thread_pool/thread_pool.h>
#include <bumbler/logger/logger.h>

using namespace bumbler;

BOOST_AUTO_TEST_SUITE(thread_pool_suite)

BOOST_AUTO_TEST_CASE(ThreadPollWait) {
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

    // wait for all tasks to finish
    tp.stop(stop_mode::WAIT_FOR_QUEUE);

    BOOST_LOG_TRIVIAL(debug) << "task_counter = " << task_counter;

    BOOST_CHECK_EQUAL(task_counter, 4);
}

#ifndef WIN32 // this test fails on windows because boost asio doesn't discard all messages on io_service.stop()

BOOST_AUTO_TEST_CASE(ThreadPoolKill) {
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

    // don't wait for tasks to finish
    // stop(false) will cancel all remining tasks but first 2 might start (2 threads in threadpool)
    // so task_counter should never be > 2
    tp.stop(stop_mode::IGNORE_QUEUE);

    BOOST_LOG_TRIVIAL(debug) << "task_counter = " << task_counter;

    BOOST_CHECK_LE(task_counter, 2);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
