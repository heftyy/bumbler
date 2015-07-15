#pragma once

#include "typed_message.h"

template<typename T>
class broadcast : public typed_message<T> {
public:
    broadcast(const actor_ref& target, const actor_ref& sender, T data)
            : typed_message<T>(target, sender, data) {
    }

    bool is_broadcast() const {
        return true;
    }
};