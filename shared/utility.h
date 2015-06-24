#pragma once

#include <iostream>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace utility {
    std::string get_current_time();

    std::string format_time(boost::posix_time::ptime time);

    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}