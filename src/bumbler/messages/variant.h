﻿#pragma once

#include <memory>
#include <boost/any.hpp>

namespace bumbler {

class variant {
public:
    virtual boost::any data() const = 0;
    virtual const boost::typeindex::type_info& type() const = 0;
    virtual ~variant() {}

    virtual std::unique_ptr<variant> clone() const = 0;
};

}