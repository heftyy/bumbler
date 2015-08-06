#pragma once

#include <atomic>
#include <memory>
#include <stdexcept>
#include <boost/any.hpp>
#include <boost/serialization/access.hpp>

class actor_ref;

enum message_type {
    BROADCAST, STOP_ACTOR, KILL_ACTOR
};

class message {
public:
    message() : type_(-1) {}

    virtual boost::any get_data() const = 0;

    virtual actor_ref& get_sender() const = 0;

    virtual actor_ref& get_target() const = 0;

    virtual std::unique_ptr<message> clone() const = 0;

    virtual void set_sender(const actor_ref& sender) = 0;

    virtual void set_target(const actor_ref& target) = 0;

    virtual ~message() {}

    virtual bool is_broadcast() const {
        return type_ == BROADCAST;
    }

    virtual bool is_stop_actor() const {
        return type_ == STOP_ACTOR;
    }

    virtual bool is_kill_actor() const {
        return type_ == KILL_ACTOR;
    }

protected:
    std::atomic<int> type_;

private:

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        throw std::runtime_error("serialize not implemented in message");
    }

};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(message)