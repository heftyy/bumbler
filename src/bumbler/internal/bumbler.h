#pragma once

#include <functional>
#include <boost/any.hpp>

using ResponseFun = std::function<void(const boost::any&)>;