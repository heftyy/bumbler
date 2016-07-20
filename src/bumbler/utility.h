#pragma once

#include <iostream>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace bumbler {

namespace utility {
    std::string get_current_time();

    std::string format_time(boost::posix_time::ptime time);

    template <typename To, typename From, typename Deleter>
    std::unique_ptr<To, Deleter> dynamic_unique_cast(std::unique_ptr<From, Deleter>&& p) {
        if (To* cast = dynamic_cast<To*>(p.get()))
        {
            std::unique_ptr<To, Deleter> result(cast, std::move(p.get_deleter()));
            p.release();
            return result;
        }
        return std::unique_ptr<To, Deleter>(nullptr); // or throw std::bad_cast() if you prefer
    }

}

}
