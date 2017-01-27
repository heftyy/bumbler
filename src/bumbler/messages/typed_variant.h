#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/any.hpp>

#include "variant.h"
#include "commands/commands.h"

namespace bumbler {

BOOST_SERIALIZATION_ASSUME_ABSTRACT(variant)

template<typename T>
class typed_variant : public variant {
public:
	typed_variant() {}

	boost::any data() const override {
		return data_;
	}

	std::unique_ptr<variant> clone() const override {
		return std::make_unique<typed_variant<T>>(*this);
	}

private:
	T data_;

	friend class typed_variant_factory;
	explicit typed_variant(const T& data) : data_(data) {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		boost::serialization::void_cast_register<typed_variant, variant>();

		ar & this->data_;
	}
};

class typed_variant_factory {
public:
	template<typename T>
	static  std::unique_ptr<variant> create(const T& data) {
		return std::unique_ptr<variant>(new typed_variant<T>(data));
	}
};

}