#include "thread_pool.h"

thread_pool::thread_pool(std::size_t pool_size)
        : work_(utility::make_unique<boost::asio::io_service::work>(io_service_)) {
    for (std::size_t i = 0; i < pool_size; ++i) {
        threads_.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));
    }
}

thread_pool::~thread_pool() {
    this->stop();
}

void thread_pool::stop(bool wait) {
    // Force all threads to return from io_service::run().
    if(wait) work_.reset();
    else io_service_.stop();

    // Suppress all exceptions.
    try {
        threads_.join_all();
    }
    catch (...) { }
}