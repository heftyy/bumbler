#pragma once

#include "typed_message.h"

class stop_actor : public typed_message<int> {
public:
    stop_actor() { }

    stop_actor(const actor_ref& target, const actor_ref& sender = actor_ref::none())
            : typed_message(target, sender, 0) { }

    bool is_stop_actor() const {
        return true;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        boost::serialization::void_cast_register<stop_actor, message>();

        ar & this->sender;
        ar & this->target;
        ar & this->data;
    }
};