#pragma once

#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>
#include <sstream>

namespace utility {
    std::string get_current_time();

    std::string format_time(boost::posix_time::ptime time);
}