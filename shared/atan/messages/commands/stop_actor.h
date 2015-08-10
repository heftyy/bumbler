#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class untyped_stop_actor {};

template<typename T>
class stop_actor : public untyped_stop_actor {
public:
    T data;

    stop_actor() {}
    stop_actor(const char* data) : data(std::string(data)) {}
    stop_actor(T data) : data(data) {}

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & this->data;
    }
};