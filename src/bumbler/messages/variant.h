#pragma once

#include <memory>
#include <boost/any.hpp>
#include <boost/serialization/access.hpp>

namespace bumbler {

class variant {
public:
	virtual boost::any data() const = 0;
	virtual ~variant() {}

	virtual std::unique_ptr<variant> clone() const = 0;
};

}