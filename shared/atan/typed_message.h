#pragma once

#include <vector>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/any.hpp>
#include "message.h"

class actor_ref;

template<typename T>
class typed_message : public message {
public:
    friend class boost::serialization::access;

    std::shared_ptr<actor_ref> target;
    std::shared_ptr<actor_ref> sender;
    T data;

    typed_message() { }

    typed_message(actor_ref& target, actor_ref& sender, T data) {
        this->target = std::make_shared<actor_ref>(target);
        this->sender = std::make_shared<actor_ref>(sender);
        this->data = data;
    }

    typed_message(std::shared_ptr<actor_ref>& target, std::shared_ptr<actor_ref>& sender, T& data) {
        this->target = target;
        this->sender = sender;
        this->data = data;
    }

    boost::any get_data() {
        return data;
    }

    actor_ref& get_sender() {
        return *sender.get();
    }

    actor_ref& get_target() {
        return *target.get();
    }

    static void restore_message(typed_message<T>& msg, std::string& received_data) {
        std::string serialized_data(received_data.begin(), received_data.end());
        std::istringstream is(serialized_data);
        boost::archive::text_iarchive in_archive(is);
        in_archive >> msg;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& *sender;
        ar& *target;
        ar& data;
    }
};