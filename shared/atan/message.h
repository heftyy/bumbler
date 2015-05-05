#pragma once

#include <memory>
#include <boost/any.hpp>

class actor_ref;

class message {
public:
    friend class boost::serialization::access;

    virtual boost::any get_data() = 0;

    virtual actor_ref& get_sender() = 0;

    virtual actor_ref& get_target() = 0;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {}
};