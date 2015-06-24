#pragma once

#include <memory>
#include <stdexcept>
#include <boost/any.hpp>

class actor_ref;

class message {
public:
    virtual boost::any get_data() = 0;

    virtual actor_ref& get_sender() = 0;

    virtual actor_ref& get_target() = 0;

    virtual void set_sender(actor_ref& sender) = 0;

    virtual void set_target(actor_ref& target) = 0;

    virtual ~message() {}

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        throw std::runtime_error("serialize not implemented in message");
    }

};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(message)