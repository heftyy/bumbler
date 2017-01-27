#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <memory>
#include "../typed_variant.h"

namespace bumbler {

class command {
public:
	std::unique_ptr<variant> var;
    int priority;

    command() { }
    command(const char* data, int priority = 0) : command(std::string(data), priority) {}
	template<typename T>
    command(const T& data, int priority = 0) : priority(priority) {
		var = typed_variant_factory::create(data);
	}

	command(const command& rhs) {
		this->var = rhs.var->clone();
		this->priority = rhs.priority;
	}

	command& operator=(const command& rhs) {
		this->var = rhs.var->clone();
		this->priority = rhs.priority;
		return *this;
	}

	command(command&&) = default; // support moving
	command& operator=(command&&) = default;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & this->var;
        ar & this->priority;
    }
};

}
