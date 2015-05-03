#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;

class logger {
    static void init() {
        /*
        logging::add_common_attributes();
        logging::core::get()->add_global_attribute("Scope", attrs::named_scope());

        logging::add_file_log
                (
                        keywords::file_name = "sample_%N.log",
                        keywords::rotation_size = 10*1024*1204,
                        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
                        keywords::format =
                                (
                                        boost::log::expressions::stream
                                        << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d_%H:%M:%S.%f")
                                        << expr::format_named_scope("Scope", keywords::format = "[%f:%l]")
                                        << ": <" << boost::log::trivial::severity << "> "
                                        << boost::log::expressions::smessage
                                )
                );
        */
    }
};