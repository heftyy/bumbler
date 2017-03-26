#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#define INFO  BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::info)
#define DEBUG BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::debug)
#define WARN  BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::warning)

#define SYS_LOGFILE "my_log"

namespace bumbler {

//Narrow-char thread-safe logger.
using logger_t = boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;

//declares a global logger with a custom initialization
BOOST_LOG_GLOBAL_LOGGER(my_logger, logger_t)

}
