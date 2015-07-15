#pragma once

#include "typed_message.h"

template<typename T>
class broadcast : public typed_message<T> {
public:
    broadcast() { }

    broadcast(const actor_ref& target, const actor_ref& sender, T data)
            : typed_message<T>(target, sender, data) {
    }

    bool is_broadcast() const {
        return true;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        boost::serialization::void_cast_register<broadcast, message>();

        ar & this->sender;
        ar & this->target;
        ar & this->data;
    }
};