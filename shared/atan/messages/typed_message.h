#pragma once

#include <vector>
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/any.hpp>
#include "message.h"

template<typename T>
class typed_message : public message {
public:
    std::shared_ptr<actor_ref> target;
    std::shared_ptr<actor_ref> sender;
    T data;

    typed_message() { }

    typed_message(const actor_ref& target, const actor_ref& sender, T data) {
        this->set_target(target);
        this->set_sender(sender);
        this->data = data;
    }

    typed_message(std::shared_ptr<actor_ref>& target, std::shared_ptr<actor_ref>& sender, T& data) {
        this->target = target;
        this->sender = sender;
        this->data = data;
    }

    boost::any get_data() const {
        return data;
    }

    actor_ref& get_sender() const {
        return *sender.get();
    }

    actor_ref& get_target() const {
        return *target.get();
    }

    void set_sender(const actor_ref& sender) {
        this->sender = std::make_shared<actor_ref>(sender);
    }

    void set_target(const actor_ref& target) {
        this->target = std::make_shared<actor_ref>(target);
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        boost::serialization::void_cast_register<typed_message, message>();

        ar & this->sender;
        ar & this->target;
        ar & this->data;
    }
};