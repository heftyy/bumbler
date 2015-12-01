#include "utility.h"

std::string utility::get_current_time() {
    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
    std::string time = utility::format_time(now);
    return time;
}

std::string utility::format_time(boost::posix_time::ptime time) {
    static std::locale loc(std::wcout.getloc(), new boost::posix_time::wtime_facet(L"%Y-%m-%d %H:%M:%S"));

    std::basic_stringstream<char> wss;
    wss.imbue(loc);
    wss << time;
    return wss.str();
}