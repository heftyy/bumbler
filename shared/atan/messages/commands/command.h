#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

template<typename T>
class command {
public:
    T data;
    int priority;

    command() { }
    command(const char* data, int priority = 0) : data(std::string(data)), priority(priority) {}
    command(T data, int priority = 0) : data(data), priority(priority) {}

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & this->data;
        ar & this->priority;
    }
};