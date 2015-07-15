#pragma once

#include <memory>
#include <stdexcept>
#include <boost/any.hpp>

class actor_ref;

class message {
public:
    virtual boost::any get_data() const = 0;

    virtual actor_ref& get_sender() const = 0;

    virtual actor_ref& get_target() const = 0;

    virtual void set_sender(const actor_ref& sender) = 0;

    virtual void set_target(const actor_ref& target) = 0;

    virtual ~message() {}

    virtual bool is_broadcast() const {
        return false;
    }

    virtual bool is_stop_actor() const {
        return false;
    }

    virtual bool is_kill_actor() const {
        return false;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        throw std::runtime_error("serialize not implemented in message");
    }

};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(message)